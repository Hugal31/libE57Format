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
   class ScaledIntegerNodeImpl : public NodeImpl
   {
   public:
      explicit ScaledIntegerNodeImpl( ImageFileImplWeakPtr destImageFile, int64_t value = 0, int64_t minimum = 0,
                                      int64_t maximum = 0, double scale = 1.0, double offset = 0.0 );

      explicit ScaledIntegerNodeImpl( ImageFileImplWeakPtr destImageFile, double scaledValue = 0.,
                                      double scaledMinimum = 0., double scaledMaximum = 0., double scale = 1.0,
                                      double offset = 0.0 );

      ~ScaledIntegerNodeImpl() override = default;

      NodeType type() const override
      {
         return E57_SCALED_INTEGER;
      }

      bool isTypeEquivalent( NodeImplSharedPtr ni ) override;
      bool isDefined( const ustring &pathName ) override;

      int64_t rawValue();
      double scaledValue();
      int64_t minimum();
      double scaledMinimum();
      int64_t maximum();
      double scaledMaximum();
      double scale();
      double offset();

      void checkLeavesInSet( const StringSet &pathNames, NodeImplSharedPtr origin ) override;

      void writeXml( ImageFileImplSharedPtr imf, CheckedFile &cf, int indent,
                     const char *forcedFieldName = nullptr ) override;

#ifdef E57_DEBUG
      void dump( int indent = 0, std::ostream &os = std::cout ) const override;
#endif

   private:
      int64_t value_;
      int64_t minimum_;
      int64_t maximum_;
      double scale_;
      double offset_;
   };
}
