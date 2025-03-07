/*
 * Original work Copyright 2009 - 2010 Kevin Ackley (kackley@gwi.net)
 * Modified work Copyright 2018 - 2020 Andy Maloney <asmaloney@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "NodeImpl.h"
#include "ImageFileImpl.h"
#include "SourceDestBufferImpl.h"
#include "StringFunctions.h"
#include "VectorNodeImpl.h"

using namespace e57;

NodeImpl::NodeImpl( ImageFileImplWeakPtr destImageFile ) : destImageFile_( destImageFile ), isAttached_( false )
{
   checkImageFileOpen( __FILE__, __LINE__,
                       static_cast<const char *>( __FUNCTION__ ) ); // does checking for all node type ctors
}

void NodeImpl::checkImageFileOpen( const char *srcFileName, int srcLineNumber, const char *srcFunctionName ) const
{
   /// Throw an exception if destImageFile (destImageFile_) isn't open
   ImageFileImplSharedPtr destImageFile( destImageFile_ );
   if ( !destImageFile->isOpen() )
   {
      throw E57Exception( E57_ERROR_IMAGEFILE_NOT_OPEN, "fileName=" + destImageFile->fileName(), srcFileName,
                          srcLineNumber, srcFunctionName );
   }
}

bool NodeImpl::isRoot() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   return parent_.expired();
};

NodeImplSharedPtr NodeImpl::parent()
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   if ( isRoot() )
   {
      /// If is root, then has self as parent (by convention)
      return shared_from_this();
   }

   NodeImplSharedPtr myParent( parent_ );

   return myParent;
}

ustring NodeImpl::pathName() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   if ( isRoot() )
   {
      return ( "/" );
   }

   NodeImplSharedPtr p( parent_ );

   if ( p->isRoot() )
   {
      return ( "/" + elementName_ );
   }

   return ( p->pathName() + "/" + elementName_ );
}

ustring NodeImpl::relativePathName( const NodeImplSharedPtr &origin, ustring childPathName ) const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );
   if ( origin == shared_from_this() )
   {
      return ( childPathName );
   }

   if ( isRoot() )
   {
      /// Got to top and didn't find origin, must be error
      throw E57_EXCEPTION2( E57_ERROR_INTERNAL,
                            "this->elementName=" + this->elementName() + " childPathName=" + childPathName );
   }

   /// Assemble relativePathName from right to left, recursively
   NodeImplSharedPtr p( parent_ );

   if ( childPathName.empty() )
   {
      return p->relativePathName( origin, elementName_ );
   }

   return p->relativePathName( origin, elementName_ + "/" + childPathName );
}

ustring NodeImpl::elementName() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   return elementName_;
}

ImageFileImplSharedPtr NodeImpl::destImageFile()
{
   /// don't checkImageFileOpen
   return ImageFileImplSharedPtr( destImageFile_ );
}

bool NodeImpl::isAttached() const
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   return isAttached_;
}

void NodeImpl::setAttachedRecursive()
{
   /// Non-terminal node types (Structure, Vector, CompressedVector) will
   /// override this virtual function, to mark their children, codecs,
   /// prototypes
   isAttached_ = true;
}

ustring NodeImpl::imageFileName() const
{
   /// don't checkImageFileOpen
   ImageFileImplSharedPtr imf( destImageFile_ );

   return imf->fileName();
}

void NodeImpl::setParent( NodeImplSharedPtr parent, const ustring &elementName )
{
   /// don't checkImageFileOpen

   /// First check if our parent_ is already set, throw
   /// E57_ERROR_ALREADY_HAS_PARENT The isAttached_ condition is to catch two
   /// errors:
   ///    1) if user attempts to use the ImageFile root as a child (e.g.
   ///    root.set("x", root)) 2) if user attempts to reuse codecs or prototype
   ///    trees of a CompressedVectorNode
   ///       ??? what if CV not attached yet?
   if ( !parent_.expired() || isAttached_ )
   {
      /// ??? does caller do setParent first, so state is not messed up when
      /// throw?
      throw E57_EXCEPTION2( E57_ERROR_ALREADY_HAS_PARENT,
                            "this->pathName=" + this->pathName() + " newParent->pathName=" + parent->pathName() );
   }

   parent_ = parent;
   elementName_ = elementName;

   /// If parent is attached then we are attached (and all of our children)
   if ( parent->isAttached() )
   {
      setAttachedRecursive();
   }
}

NodeImplSharedPtr NodeImpl::getRoot()
{
   /// don't checkImageFileOpen
   NodeImplSharedPtr p( shared_from_this() );
   while ( !p->isRoot() )
   {
      p = NodeImplSharedPtr( p->parent_ ); //??? check if bad ptr?
   }

   return p;
}

//??? use visitor?
bool NodeImpl::isTypeConstrained()
{
   /// don't checkImageFileOpen
   /// A node is type constrained if any of its parents is an homo VECTOR or
   /// COMPRESSED_VECTOR with more than one child
   NodeImplSharedPtr p( shared_from_this() );

   while ( !p->isRoot() )
   {
      /// We have a parent since we are not root
      p = NodeImplSharedPtr( p->parent_ ); //??? check if bad ptr?

      switch ( p->type() )
      {
         case E57_VECTOR:
         {
            /// Downcast to shared_ptr<VectorNodeImpl>
            std::shared_ptr<VectorNodeImpl> ai( std::static_pointer_cast<VectorNodeImpl>( p ) );

            /// If homogeneous vector and have more than one child, then can't
            /// change them
            if ( !ai->allowHeteroChildren() && ai->childCount() > 1 )
            {
               return ( true );
            }
         }
         break;
         case E57_COMPRESSED_VECTOR:
            /// Can't make any type changes to CompressedVector prototype.  ???
            /// what if hasn't been written to yet
            return ( true );
         default:
            break;
      }
   }
   /// Didn't find any constraining VECTORs or COMPRESSED_VECTORs in path above
   /// us, so our type is not constrained.
   return ( false );
}

NodeImplSharedPtr NodeImpl::get( const ustring &pathName )
{
   /// This is common virtual function for terminal E57 element types: Integer,
   /// ScaledInteger, Float, Blob. The non-terminal types override this virtual
   /// function. Only absolute pathNames make any sense here, because the
   /// terminal types can't have children, so relative pathNames are illegal.

#ifdef E57_DEBUG
   _verifyPathNameAbsolute( pathName );
#endif

   NodeImplSharedPtr root = _verifyAndGetRoot();

   /// Forward call to the non-terminal root node
   return root->get( pathName );
}

void NodeImpl::set( const ustring &pathName, NodeImplSharedPtr ni, bool autoPathCreate )
{
   /// This is common virtual function for terminal E57 element types: Integer,
   /// ScaledInteger, Float, Blob. The non-terminal types override this virtual
   /// function. Only absolute pathNames make any sense here, because the
   /// terminal types can't have children, so relative pathNames are illegal.

#ifdef E57_DEBUG
   _verifyPathNameAbsolute( pathName );
#endif

   NodeImplSharedPtr root = _verifyAndGetRoot();

   /// Forward call to the non-terminal root node
   root->set( pathName, ni, autoPathCreate );
}

void NodeImpl::set( const StringList & /*fields*/, unsigned /*level*/, NodeImplSharedPtr /*ni*/,
                    bool /*autoPathCreate*/ )
{
   /// If get here, then tried to call set(fields...) on NodeImpl that wasn't a
   /// StructureNodeImpl, so that's an error
   throw E57_EXCEPTION1( E57_ERROR_BAD_PATH_NAME ); //???
}

void NodeImpl::checkBuffers( const std::vector<SourceDestBuffer> &sdbufs,
                             bool allowMissing ) //??? convert sdbufs to vector of shared_ptr
{
   /// this node is prototype of CompressedVector

   /// don't checkImageFileOpen

   StringSet pathNames;

   for ( unsigned i = 0; i < sdbufs.size(); i++ )
   {
      ustring pathName = sdbufs.at( i ).impl()->pathName();

      /// Check that all buffers are same size
      if ( sdbufs.at( i ).impl()->capacity() != sdbufs.at( 0 ).impl()->capacity() )
      {
         throw E57_EXCEPTION2( E57_ERROR_BUFFER_SIZE_MISMATCH,
                               "this->pathName=" + this->pathName() + " sdbuf.pathName=" + pathName +
                                  " firstCapacity=" + toString( sdbufs.at( 0 ).impl()->capacity() ) +
                                  " secondCapacity=" + toString( sdbufs.at( i ).impl()->capacity() ) );
      }

      /// Add each pathName to set, error if already in set (a duplicate
      /// pathName in sdbufs)
      if ( !pathNames.insert( pathName ).second )
      {
         throw E57_EXCEPTION2( E57_ERROR_BUFFER_DUPLICATE_PATHNAME,
                               "this->pathName=" + this->pathName() + " sdbuf.pathName=" + pathName );
      }

      /// Check no bad fields in sdbufs
      if ( !isDefined( pathName ) )
      {
         throw E57_EXCEPTION2( E57_ERROR_PATH_UNDEFINED,
                               "this->pathName=" + this->pathName() + " sdbuf.pathName=" + pathName );
      }
   }

   if ( !allowMissing )
   {
      /// Traverse tree recursively, checking that all nodes are listed in
      /// sdbufs
      checkLeavesInSet( pathNames, shared_from_this() );
   }
}

bool NodeImpl::findTerminalPosition( const NodeImplSharedPtr &target, uint64_t &countFromLeft )
{
   /// don't checkImageFileOpen

   if ( this == &*target ) //??? ok?
   {
      return true;
   }

   switch ( type() )
   {
      case E57_STRUCTURE:
      {
         auto sni = static_cast<StructureNodeImpl *>( this );

         /// Recursively visit child nodes
         int64_t childCount = sni->childCount();
         for ( int64_t i = 0; i < childCount; ++i )
         {
            if ( sni->get( i )->findTerminalPosition( target, countFromLeft ) )
            {
               return true;
            }
         }
      }
      break;

      case E57_VECTOR:
      {
         auto vni = static_cast<VectorNodeImpl *>( this );

         /// Recursively visit child nodes
         int64_t childCount = vni->childCount();
         for ( int64_t i = 0; i < childCount; ++i )
         {
            if ( vni->get( i )->findTerminalPosition( target, countFromLeft ) )
            {
               return true;
            }
         }
      }
      break;

      case E57_COMPRESSED_VECTOR:
         break; //??? for now, don't search into contents of compressed vector

      case E57_INTEGER:
      case E57_SCALED_INTEGER:
      case E57_FLOAT:
      case E57_STRING:
      case E57_BLOB:
         countFromLeft++;
         break;
   }

   return ( false );
}

#ifdef E57_DEBUG
void NodeImpl::dump( int indent, std::ostream &os ) const
{
   /// don't checkImageFileOpen
   os << space( indent ) << "elementName: " << elementName_ << std::endl;
   os << space( indent ) << "isAttached:  " << isAttached_ << std::endl;
   os << space( indent ) << "path:        " << pathName() << std::endl;
}

bool NodeImpl::_verifyPathNameAbsolute( const ustring &inPathName )
{
   checkImageFileOpen( __FILE__, __LINE__, static_cast<const char *>( __FUNCTION__ ) );

   /// Parse to determine if pathName is absolute
   bool isRelative = false;
   std::vector<ustring> fields;
   ImageFileImplSharedPtr imf( destImageFile_ );

   imf->pathNameParse( inPathName, isRelative,
                       fields ); // throws if bad pathName

   /// If not an absolute path name, have error
   if ( isRelative )
   {
      throw E57_EXCEPTION2( E57_ERROR_BAD_PATH_NAME, "this->pathName=" + this->pathName() + " pathName=" + inPathName );
   }

   return isRelative;
}
#endif

NodeImplSharedPtr NodeImpl::_verifyAndGetRoot()
{
   /// Find root of the tree
   NodeImplSharedPtr root( shared_from_this()->getRoot() );

   /// Check to make sure root node is non-terminal type (otherwise have stack
   /// overflow).
   switch ( root->type() )
   {
      case E57_STRUCTURE:
      case E57_VECTOR: //??? COMPRESSED_VECTOR?
         break;
      default:
         throw E57_EXCEPTION2( E57_ERROR_INTERNAL, "root invalid for this->pathName=" + this->pathName() );
   }

   return root;
}
