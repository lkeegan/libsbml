/**
 * \file    XMLOutputStream.cpp
 * \brief   XMLOutputStream
 * \author  Ben Bornstein
 *
 * $Id$
 * $Source$
 */
/* Copyright 2006 California Institute of Technology and Japan Science and
 * Technology Corporation.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.  A copy of the license agreement is
 * provided in the file named "LICENSE.txt" included with this software
 * distribution.  It is also available online at
 * http://sbml.org/software/libsbml/license.html
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 */


#include <locale>
#include <string>

#include "XMLTriple.h"
#include "XMLOutputStream.h"


using namespace std;


/**
 * Creates a new XMLOutputStream that wraps stream.
 */
XMLOutputStream::XMLOutputStream (  ostream&       stream
                                  , const string&  encoding
                                  , bool           writeXMLDecl ) :
   mInStart ( false    )
 , mDoIndent( true     )
 , mIndent  ( 0        )
 , mStream  ( stream   )
 , mEncoding( encoding )
{
  mStream.imbue( locale::classic() );
  if (writeXMLDecl) this->writeXMLDecl();
}


/**
 * Decreases the indentation level for this XMLOutputStream.
 */
void
XMLOutputStream::downIndent ()
{
  if (mIndent) --mIndent;
}


/**
 * Writes the given XML end element name to this XMLOutputStream.
 */
void
XMLOutputStream::endElement (const std::string& name)
{
  if (mInStart)
  {
    mInStart = false;
    mStream << '/' << '>';
  }
  else
  {
    downIndent();
    writeIndent(true); 

    mStream << '<' << '/';
    writeName(name);
    mStream << '>';
  }
}


/**
 * Writes the given XML end element 'prefix:name' to this
 * XMLOutputStream.
 */
void
XMLOutputStream::endElement (const XMLTriple& triple)
{
  if (mInStart)
  {
    mInStart = false;
    mStream << '/' << '>';
  }
  else
  {
    downIndent();
    writeIndent(true); 

    mStream << '<' << '/';
    writeName(triple);
    mStream << '>';
  }
}


/**
 * Turns automatic indentation on or off for this XMLOutputStream.
 */
void
XMLOutputStream::setAutoIndent (bool indent)
{
  mDoIndent = indent;
}


/**
 * Writes the given XML start element name to this XMLOutputStream.
 */
void
XMLOutputStream::startElement (const string& name)
{
  if (mInStart)
  {
    mStream << '>';
    upIndent();
  }

  mInStart = true;

  writeIndent();

  mStream << '<';
  writeName(name);
}


/**
 * Writes the given XML start element 'prefix:name' to this
 * XMLOutputStream.
 */
void
XMLOutputStream::startElement (const XMLTriple& triple)
{
  if (mInStart)
  {
    mStream << '>';
    upIndent();
  }

  mInStart = true;

  writeIndent();

  mStream << '<';
  writeName(triple);
}


/**
 * Increases the indentation level for this XMLOutputStream.
 */
void
XMLOutputStream::upIndent ()
{
  ++mIndent;
}


/**
 * Writes the given attribute, name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const string& value)
{
  if ( value.empty() ) return;

  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="value" to this
 * XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const XMLTriple& triple, const string& value)
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Writes the given attribute, name="true" or name="false" to this
 * XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const bool& value)
{
  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="true" or prefix:name="false" to
 * this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const XMLTriple& triple, const bool& value)
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Writes the given attribute, name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const double& value)
{
  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const XMLTriple& triple, const double& value)
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Writes the given attribute, name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const long& value)
{
  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const XMLTriple& triple, const long& value)
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Writes the given attribute, name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const int& value)
{
  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="value" to this
 * XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const XMLTriple& triple, const int& value)
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Writes the given attribute, name="value" to this XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (const string& name, const unsigned int& value)
{
  mStream << ' ';

  writeName ( name  );
  writeValue( value );
}


/**
 * Writes the given attribute, prefix:name="value" to this
 * XMLOutputStream.
 */
void
XMLOutputStream::writeAttribute (  const XMLTriple&     triple
                                 , const unsigned int&  value )
{
  mStream << ' ';

  writeName ( triple );
  writeValue( value  );
}


/**
 * Outputs indentation whitespace.
 */
void
XMLOutputStream::writeIndent (bool isEnd)
{
  if (mDoIndent)
  {
    if (mIndent > 0 || isEnd) mStream << endl;
    for (unsigned int n = 0; n < mIndent; ++n) mStream << ' ' << ' ';
  }
}


/**
 * Outputs the given characters to the underlying stream.
 */
void
XMLOutputStream::writeChars (const string& chars)
{
  for (string::const_iterator c = chars.begin(); c != chars.end(); ++c)
  {
    *this << *c;
  }
}


/**
 * Outputs name.
 */
void
XMLOutputStream::writeName (const string& name)
{
  writeChars(name);
}


/**
 * Outputs prefix:name.
 */
void
XMLOutputStream::writeName (const XMLTriple& triple)
{
  if ( !triple.getPrefix().empty() )
  {
    writeChars( triple.getPrefix() );
    mStream << ':';
  }

  writeChars( triple.getName() );
}


/**
 * Outputs value in quotes.
 */
void
XMLOutputStream::writeValue (const string& value)
{
  mStream << '=' << '"';
  writeChars(value);
  mStream << '"';
}


/**
 * Outputs "true" or "false" in quotes.
 */
void
XMLOutputStream::writeValue (const bool& value)
{
  mStream << '=' << '"' << (value ? "true" : "false") << '"';
}


/**
 * Outputs the double value in quotes, or "INF", "-INF", or "NaN".
 */
void
XMLOutputStream::writeValue (const double& value)
{
  mStream << '=' << '"';

  if (value != value)
  {
    mStream << "NaN";
  }
  else if (value == numeric_limits<double>::infinity())
  {
    mStream << "INF";
  }
  else if (value == - numeric_limits<double>::infinity())
  {
    mStream << "-INF";
  }
  else
  {
    mStream << value;
  }

  mStream << '"';
}


/**
 * Outputs the long value in quotes.
 */
void
XMLOutputStream::writeValue (const long& value)
{
  mStream << '=' << '"' << value << '"';
}


/**
 * Outputs the int value in quotes.
 */
void
XMLOutputStream::writeValue (const int& value)
{
  mStream << '=' << '"' << value << '"';
}


/**
 * Outputs the int value in quotes.
 */
void
XMLOutputStream::writeValue (const unsigned int& value)
{
  mStream << '=' << '"' << value << '"';
}


/**
 * Writes the XML declaration:
 * <?xml version="1.0" encoding="..."?>
 */
void
XMLOutputStream::writeXMLDecl ()
{
  mStream << "<?xml version=\"1.0\"";

  if ( !mEncoding.empty() ) writeAttribute("encoding", mEncoding);

  mStream << "?>";
  mStream << endl;
}


/**
 * Outputs the given characters to the underlying stream.
 */
XMLOutputStream&
XMLOutputStream::operator<< (const string& chars)
{
  if (mInStart)
  {
    mInStart = false;
    mStream << '>';
  }

  writeChars(chars);

  return *this;
}
