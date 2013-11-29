/**
 * Copyright 2013 Automatak, LLC
 *
 * Licensed to Automatak, LLC (www.automatak.com) under one or more
 * contributor license agreements. See the NOTICE file distributed with this
 * work for additional information regarding copyright ownership. Automatak, LLC
 * licenses this file to you under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0.html
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */
package com.automatak.render.cpp

import com.automatak.render._

object EnumFromType {
  def signature(enum: EnumModel) = List(enum.name, List(enum.name, "FromType(", getType(enum.enumType)," arg)").mkString).mkString(" ")

  case class HeaderRender(i: Indentation) extends ModelRenderer[EnumModel] {
    def render(em: EnumModel) : Iterator[String] = {
      Iterator(signature(em)+";")
    }
  }

  case class ImplRender(indent: Indentation) extends ModelRenderer[EnumModel] {

    def render(em: EnumModel) : Iterator[String] = {

      def header = Iterator(signature(em))
      def smr = new ReturnSwitchModelRenderer[EnumValue](indent)(ev => em.render(ev.value))(ev => em.qualified(ev))
      def switch = smr.render(em.values)
      def returnDefault = Iterator(List("return ", em.qualified(em.default),";").mkString)

      header ++ bracket(indent) {
        switch ++
          returnDefault
      }
    }
  }
}


