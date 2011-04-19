#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# This file is part of the Shiboken Python Bindings Generator project.
#
# Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
#
# Contact: PySide team <contact@pyside.org>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation. Please
# review the following information to ensure the GNU Lesser General
# Public License version 2.1 requirements will be met:
# http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
# #
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA

'''Test cases for user added fields.'''

import unittest
import sample

class ModuleAddedFieldTest(unittest.TestCase):
    def testAddedIntField(self):
        self.assert_(hasattr(sample, 'addedField_int'))
        self.assertEqual(type(sample.addedField_int), int)
        self.assertEqual(sample.addedField_int, 4321)

    def testAddedFloatField(self):
        self.assert_(hasattr(sample, 'addedField_float'))
        self.assertEqual(type(sample.addedField_float), float)
        self.assertAlmostEqual(sample.addedField_float, 21.0)

    def testAddedStringLiteralField(self):
        self.assert_(hasattr(sample, 'addedField_stringLiteral'))
        self.assertEqual(type(sample.addedField_stringLiteral), str)
        self.assertEqual(sample.addedField_stringLiteral, 'foobar')

    def testAddedStringFunctionField(self):
        self.assert_(hasattr(sample, 'addedField_stringFunction'))
        self.assertEqual(type(sample.addedField_stringFunction), str)
        self.assertEqual(sample.addedField_stringFunction, 'field from string function')

    def testAddedObjectField(self):
        self.assert_(hasattr(sample, 'addedField_object'))
        self.assertEqual(type(sample.addedField_object), sample.Point)
        self.assertEqual(sample.addedField_object, sample.Point(1, 2))


class ClassAddedStaticFieldTest(unittest.TestCase):
    def testAddedIntField(self):
        self.assert_(hasattr(sample.Echo, 'addedField_int'))
        self.assertEqual(type(sample.Echo.addedField_int), int)
        self.assertEqual(sample.Echo.addedField_int, 4321)

    def testAddedFloatField(self):
        self.assert_(hasattr(sample.Echo, 'addedField_float'))
        self.assertEqual(type(sample.Echo.addedField_float), float)
        self.assertAlmostEqual(sample.Echo.addedField_float, 21.0)

    def testAddedStringLiteralField(self):
        self.assert_(hasattr(sample.Echo, 'addedField_stringLiteral'))
        self.assertEqual(type(sample.Echo.addedField_stringLiteral), str)
        self.assertEqual(sample.Echo.addedField_stringLiteral, 'foobar')

    def testAddedStringFunctionField(self):
        self.assert_(hasattr(sample.Echo, 'addedField_stringFunction'))
        self.assertEqual(type(sample.Echo.addedField_stringFunction), str)
        self.assertEqual(sample.Echo.addedField_stringFunction, 'field from string function')

    def testAddedObjectField(self):
        self.assert_(hasattr(sample.Echo, 'addedField_object'))
        self.assertEqual(type(sample.Echo.addedField_object), sample.Str)
        self.assertEqual(sample.Echo.addedField_object, sample.Str("Str-foobar"))


if __name__ == '__main__':
    unittest.main()
