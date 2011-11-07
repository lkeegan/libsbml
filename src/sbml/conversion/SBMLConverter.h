/**
 * @file    SBMLConverter.h
 * @brief   Definition of SBMLConverter, the base class for SBML conversion.
 * @author  Sarah Keating
 * 
 * <!--------------------------------------------------------------------------
 * This file is part of libSBML.  Please visit http://sbml.org for more
 * information about SBML, and the latest version of libSBML.
 *
 * Copyright (C) 2009-2011 jointly by the following organizations: 
 *     1. California Institute of Technology, Pasadena, CA, USA
 *     2. EMBL European Bioinformatics Institute (EBML-EBI), Hinxton, UK
 *  
 * Copyright (C) 2006-2008 by the California Institute of Technology,
 *     Pasadena, CA, USA 
 *  
 * Copyright (C) 2002-2005 jointly by the following organizations: 
 *     1. California Institute of Technology, Pasadena, CA, USA
 *     2. Japan Science and Technology Agency, Japan
 * 
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.  A copy of the license agreement is provided
 * in the file named "LICENSE.txt" included with this software distribution
 * and also available online as http://sbml.org/software/libsbml/license.html
 * ------------------------------------------------------------------------ -->
 *
 * @class SBMLConverter
 * @brief Base class for SBML converters.
 *
 * @htmlinclude not-sbml-warning.html
 *
 * The SBMLConverter class is the base class for the various SBML @em
 * converters: classes of objects that transform or convert SBML documents.
 * These transformations can involve essentially anything that can be
 * written algorithmically; examples include converting the units of
 * measurement in a model, or converting from one Level+Version combination
 * of SBML to another.
 *
 * LibSBML provides a number of built-in converters, and applications can
 * create their own by subclassing SBMLConverter and following the examples
 * of the existing converters.  The following are the built-in converters
 * in libSBML @htmlinclude libsbml-version.html:
 * @li SBMLFunctionDefinitionConverter
 * @li SBMLInitialAssignmentConverter
 * @li SBMLLevelVersionConverter
 * @li SBMLRuleConverter
 * @li SBMLStripPackageConverter
 * @li SBMLUnitsConverter
 */

#ifndef SBMLConverter_h
#define SBMLConverter_h

#include <sbml/SBMLNamespaces.h>
#include <sbml/conversion/ConversionProperties.h>
#include <sbml/SBMLTypes.h>


#ifdef __cplusplus


LIBSBML_CPP_NAMESPACE_BEGIN


class LIBSBML_EXTERN SBMLConverter
{
public:

  /**
   * Creates a new SBMLConverter object.
   */
  SBMLConverter ();


  /**
   * Copy constructor; creates a copy of an SBMLConverter object.
   *
   * @param c the SBMLConverter object to copy.
   */
  SBMLConverter(const SBMLConverter& c);


  /**
   * Destroy this SBMLConverter object.
   */
  virtual ~SBMLConverter ();


  /**
   * Assignment operator for SBMLConverter.
   */
  SBMLConverter& operator=(const SBMLConverter&);


  /**
   * Creates and returns a deep copy of this SBMLConverter object.
   * 
   * @return a (deep) copy of this SBMLConverter object.
   */
  virtual SBMLConverter* clone() const;


  /**
   * Returns the SBML document that is the subject of the conversions.
   *
   * @return the current SBMLDocument object.
   */
  virtual SBMLDocument* getDocument();


  /**
   * Returns the SBML document that is the subject of the conversions.
   *
   * @return the current SBMLDocument object, as a const reference.
   */
  virtual const SBMLDocument* getDocument() const;


  /**
   * Returns the default properties of this converter.
   *
   * A given converter exposes one or more properties that can be adjusted
   * in order to influence the behavior of the converter.  This method
   * returns the @em default property settings for this converter.  The
   * default properties are set when the converter object is created.  The
   * defaults themselves cannot be changed, but the run-time properties of 
   * the converter object can be adjusted by using the method
   * setProperties(const ConversionProperties *props).
   * 
   * @return default properties for the converter
   *
   * @see setProperties()
   * @see matchesProperties()
   */
  virtual ConversionProperties getDefaultProperties() const;


  /**
   * Returns the target SBML namespaces of the currently set properties.
   *
   * SBML namespaces are used by libSBML to express the Level+Version of
   * the SBML document (and, possibly, any SBML Level&nbsp;3 packages in
   * use). Some converters' behavior is affected by the SBML namespace
   * configured in the converter.  For example, the actions of
   * SBMLLevelVersionConverter, the converter for converting SBML documents
   * from one Level+Version combination to another, are fundamentally
   * dependent on the SBML namespaces being targetted.
   *
   * @return the SBMLNamespaces object that describes the SBML namespaces
   * in effect.
   */
  virtual SBMLNamespaces* getTargetNamespaces();


  /**
   * Predicate returning @c true if this converter's properties matches a
   * given set of configuration properties.
   * 
   * @param props the configuration properties to match.
   * 
   * @return @c true if this converter's properties match, @c false
   * otherwise.
   */
  virtual bool matchesProperties(const ConversionProperties &props) const;


  /** 
   * Sets the current SBML document to the given SBMLDocument object.
   * 
   * @param doc the document to use for this conversion.
   * 
   * @return integer value indicating the success/failure of the operation.
   * @if clike The value is drawn from the enumeration
   * #OperationReturnValues_t. @endif The set of possible values that may
   * be returned ultimately depends on the specific subclass of
   * SBMLConverter being used, but the default method can return the
   * following values:
   * @li @link OperationReturnValues_t#LIBSBML_OPERATION_SUCCESS LIBSBML_OPERATION_SUCCESS @endlink
   */
  virtual int setDocument(const SBMLDocument* doc);


  /** 
   * Sets the configuration properties to be used by this converter.
   * 
   * A given converter exposes one or more properties that can be adjusted
   * in order to influence the behavior of the converter.  This method sets
   * the current properties for this converter.
   *
   * @param props the properties
   * 
   * @return integer value indicating the success/failure of the operation.
   * @if clike The value is drawn from the enumeration
   * #OperationReturnValues_t. @endif The set of possible values that may
   * be returned ultimately depends on the specific subclass of
   * SBMLConverter being used, but the default method can return the
   * following values:
   * @li @link OperationReturnValues_t#LIBSBML_OPERATION_SUCCESS LIBSBML_OPERATION_SUCCESS @endlink
   * @li @link OperationReturnValues_t#LIBSBML_OPERATION_FAILED LIBSBML_OPERATION_FAILED @endlink
   *
   * @see getProperties()
   * @see matchesProperties()
   */  
  virtual int setProperties(const ConversionProperties *props);


  /** 
   * Returns the current properties in effect for this converter.
   * 
   * A given converter exposes one or more properties that can be adjusted
   * in order to influence the behavior of the converter.  This method
   * returns the current properties for this converter; in other words, the
   * settings in effect at this moment.  To change the property values, you
   * can use setProperties(const ConversionProperties *props).
   * 
   * @return the currently set configuration properties 
   *
   * @see setProperties()
   * @see matchesProperties()
   */
  virtual ConversionProperties* getProperties() const;


  /** 
   * Perform the conversion.
   *
   * This method causes the converter to do the actual conversion
   * work, that is, to convert the SBMLDocument object set by
   * setDocument(const SBMLDocument* doc) and with the configuration
   * options set by setProperties(const ConversionProperties *props).
   * 
   * @return  integer value indicating the success/failure of the operation.
   * @if clike The value is drawn from the enumeration
   * #OperationReturnValues_t. @endif The set of possible values that may
   * be returned depends on the converter subclass; please consult
   * the documentation for the relevant class to find out what the
   * possibilities are.
   */
  virtual int convert(); 


#ifndef SWIG

#endif // SWIG


protected:
  /** @cond doxygen-libsbml-internal */

  SBMLDocument *   mDocument;
  ConversionProperties *mProps;

  friend class SBMLDocument;
  /** @endcond */


private:
  /** @cond doxygen-libsbml-internal */


  /** @endcond */
};

LIBSBML_CPP_NAMESPACE_END

#endif  /* __cplusplus */


#ifndef SWIG

LIBSBML_CPP_NAMESPACE_BEGIN
BEGIN_C_DECLS


END_C_DECLS
LIBSBML_CPP_NAMESPACE_END

#endif  /* !SWIG */
#endif  /* SBMLConverter_h */

