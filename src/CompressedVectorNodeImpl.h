#pragma once
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

namespace e57
{
   class CompressedVectorNodeImpl : public NodeImpl
   {
   public:
      explicit CompressedVectorNodeImpl( ImageFileImplWeakPtr destImageFile );
      ~CompressedVectorNodeImpl() override = default;

      NodeType type() const override
      {
         return E57_COMPRESSED_VECTOR;
      }

      bool isTypeEquivalent( NodeImplSharedPtr ni ) override;
      bool isDefined( const ustring &pathName ) override;
      void setAttachedRecursive() override;

      void setPrototype( const NodeImplSharedPtr &prototype );
      NodeImplSharedPtr getPrototype() const;
      void setCodecs( const std::shared_ptr<VectorNodeImpl> &codecs );
      std::shared_ptr<VectorNodeImpl> getCodecs() const;

      int64_t childCount() const;

      void checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin ) override;

      void writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
                     const char *forcedFieldName = nullptr ) override;

      /// Iterator constructors
      std::shared_ptr<CompressedVectorWriterImpl> writer( std::vector<SourceDestBuffer> sbufs );
      std::shared_ptr<CompressedVectorReaderImpl> reader( std::vector<SourceDestBuffer> dbufs );

      int64_t getRecordCount() const
      {
         return ( recordCount_ );
      }

      uint64_t getBinarySectionLogicalStart() const
      {
         return ( binarySectionLogicalStart_ );
      }

      void setRecordCount( int64_t recordCount )
      {
         recordCount_ = recordCount;
      }

      void setBinarySectionLogicalStart( uint64_t binarySectionLogicalStart )
      {
         binarySectionLogicalStart_ = binarySectionLogicalStart;
      }

#ifdef E57_DEBUG
      void dump( int indent = 0, std::ostream &os = std::cout ) const override;
#endif

   private:
      friend class CompressedVectorReaderImpl;

      NodeImplSharedPtr prototype_;
      std::shared_ptr<VectorNodeImpl> codecs_;

      int64_t recordCount_ = 0;
      uint64_t binarySectionLogicalStart_ = 0;
   };
}
