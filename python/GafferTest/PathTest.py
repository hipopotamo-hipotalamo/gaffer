##########################################################################
#  
#  Copyright (c) 2011, John Haddon. All rights reserved.
#  Copyright (c) 2012, Image Engine Design Inc. All rights reserved.
#  
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are
#  met:
#  
#      * Redistributions of source code must retain the above
#        copyright notice, this list of conditions and the following
#        disclaimer.
#  
#      * Redistributions in binary form must reproduce the above
#        copyright notice, this list of conditions and the following
#        disclaimer in the documentation and/or other materials provided with
#        the distribution.
#  
#      * Neither the name of John Haddon nor the names of
#        any other contributors to this software may be used to endorse or
#        promote products derived from this software without specific prior
#        written permission.
#  
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
#  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
#  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
#  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
#  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#  
##########################################################################

import unittest

import IECore

import Gaffer

class PathTest( unittest.TestCase ) :

	def test( self ) :
	
		self.assertRaises( ValueError, Gaffer.Path, "noStartingSlash" )

		p = Gaffer.Path( "/" )
		self.assertEqual( len( p ), 0 )
		self.assertEqual( str( p ), "/" )
		
		p = Gaffer.Path( "/a" )
		self.assertEqual( len( p ), 1 )
		self.assertEqual( p[0], "a" )
		self.assertEqual( str( p ), "/a" )

		p = Gaffer.Path( "/a//b/" )
		self.assertEqual( len( p ), 2 )
		self.assertEqual( p[0], "a" )
		self.assertEqual( p[1], "b" )
		self.assertEqual( str( p ), "/a/b" )
		
		p = Gaffer.Path( [ "a", "b" ] )
		self.assertEqual( len( p ), 2 )
		self.assertEqual( p[0], "a" )
		self.assertEqual( p[1], "b" )
		self.assertEqual( str( p ), "/a/b" )
		
	def testChangedSignal( self ) :
	
		changedPaths = []
		def f( path ) :		
			changedPaths.append( str( path ) )
	
		p = Gaffer.Path( "/" )
		c = p.pathChangedSignal().connect( f )
		
		p.append( "hello" )
		p.append( "goodbye" )
		p[0] = "hello"
		p[1] = "bob"
		
		self.assertEqual( changedPaths, [ "/hello", "/hello/goodbye", "/hello/bob" ] )
		
	def testFilters( self ) :
	
		p = Gaffer.Path( "/" )
		self.assertEqual( p.getFilter(), None )
		
		changedPaths = []
		def f( path ) :		
			changedPaths.append( str( path ) )
	
		c = p.pathChangedSignal().connect( f )
		self.assertEqual( len( changedPaths ), 0 )
		
		filter = Gaffer.FileNamePathFilter( [ "*.gfr" ] )
		
		p.setFilter( filter )
		self.failUnless( p.getFilter() is filter )
		self.assertEqual( len( changedPaths ), 1 )
		
		p.setFilter( filter )
		self.failUnless( p.getFilter() is filter )
		self.assertEqual( len( changedPaths ), 1 )
		
		p.setFilter( None )
		self.failUnless( p.getFilter() is None )
		self.assertEqual( len( changedPaths ), 2 )
		
		p.setFilter( filter )
		self.failUnless( p.getFilter() is filter )
		self.assertEqual( len( changedPaths ), 3 )
		
		filter.setEnabled( False )
		self.assertEqual( len( changedPaths ), 4 )
		
		filter.setEnabled( True )
		self.assertEqual( len( changedPaths ), 5 )

	def testConstructWithFilter( self ) :
	
		p = Gaffer.Path( "/test/path" )
		self.failUnless( p.getFilter() is None )
		
		f = Gaffer.FileNamePathFilter( [ "*.exr" ] )
		p = Gaffer.Path( "/test/path", filter = f )
		self.failUnless( p.getFilter() is f )
		
if __name__ == "__main__":
	unittest.main()
	
