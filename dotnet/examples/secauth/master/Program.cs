﻿
//
// Licensed to Green Energy Corp (www.greenenergycorp.com) under one or
// more contributor license agreements. See the NOTICE file distributed
// with this work for additional information regarding copyright ownership.
// Green Energy Corp licenses this file to you under the Apache License,
// Version 2.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project was forked on 01/01/2013 by Automatak, LLC and modifications
// may have been made to this file. Automatak, LLC licenses these modifications
// to you under the terms of the License.
//
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

using Automatak.DNP3.Adapter;
using Automatak.DNP3.Interface;

class MasterApplicatonSA : DefaultMasterApplication, IMasterApplicationSA
{
    void IMasterApplication.OnTaskComplete(TaskInfo info)
    {        
        Console.WriteLine(String.Format("Session key change {0} for user {1}", info.result, info.user.Number));        
    }


    void IMasterApplicationSA.PersistNewUpdateKey(string username, User user, UpdateKey key)
    {
        
    }
   
}

namespace DotNetMasterDemo
{
    class Program
    {
        static int Main(string[] args)
        {
            var application = new MasterApplicatonSA();

            IDNP3Manager mgr = DNP3ManagerFactory.CreateManager();

            Console.WriteLine(String.Format("Crypto: {0}", mgr.SSLVersion()));


            mgr.AddLogHandler(PrintingLogAdapter.Instance); //this is optional
            var channel = mgr.AddTCPClient("client", LogLevels.NORMAL, ChannelRetry.Default, "127.0.0.1", 20000);

            //optionally, add a listener for the channel state
            channel.AddStateListener(state => Console.WriteLine("channel state: " + state));

            var config = new MasterStackConfig();

            //setup your stack configuration here.
            config.link.localAddr = 1;
            config.link.remoteAddr = 10;

            var master = channel.AddMasterSA("master", PrintingSOEHandler.Instance, application, config);

            // define users on the master
            master.AddUser(User.Default, UpdateKey.Demo(0xFF, KeyWrapAlgorithm.AES_128));

            // you a can optionally add various kinds of polls
            var integrityPoll = master.AddClassScan(ClassField.AllClasses, TimeSpan.FromMinutes(1), TaskConfig.Default);
            var rangePoll = master.AddRangeScan(30, 2, 5, 7, TimeSpan.FromSeconds(20), TaskConfig.Default);
            var classPoll = master.AddClassScan(ClassField.AllEventClasses, TimeSpan.FromSeconds(5), TaskConfig.Default);

            /* you can also do very custom scans
            var headers = new Header[] { Header.Range8(1, 2, 7, 8), Header.Count8(2, 3, 7) };
            var weirdPoll = master.AddScan(headers, TimeSpan.FromSeconds(20));
            */

            master.Enable(); // enable communications

            Console.WriteLine("Enter a command");

            while (true)
            {
                switch (Console.ReadLine())
                {
                    case "a":
                        // perform an ad-hoc scan of all analogs
                        master.ScanAllObjects(30, 0, TaskConfig.Default);
                        break;
                    case "c":
                        var crob = new ControlRelayOutputBlock(ControlCode.PULSE_ON, 1, 100, 100);
                        var commands = CommandHeader.From(IndexedValue.From(crob, 0));
                        var task = master.SelectAndOperate(commands, TaskConfig.With(User.Default));
                        task.ContinueWith((result) => Console.WriteLine("Result: " + result.Result));
                        break;
                    case "l":
                        // add interpretation to the current logging level
                        var filters = channel.GetLogFilters();
                        channel.SetLogFilters(filters.Add(LogFilters.TRANSPORT_TX | LogFilters.TRANSPORT_RX));
                        break;
                    case "i":
                        integrityPoll.Demand();
                        break;
                    case "r":
                        rangePoll.Demand();
                        break;
                    case "e":
                        classPoll.Demand();
                        break;
                    case "x":
                        return 0;
                    default:
                        break;
                }
            }
        }        
    }
}
