/**
 * @cond doxygen-libsbml-internal
 *
 * @file    IdentifierConsistencyConstraints.cpp
 * @brief   IdentifierConsistency check constraints.  See SBML Wiki
 * @author  Sarah Keating
 * 
 * <!--------------------------------------------------------------------------
 * This file is part of libSBML.  Please visit http://sbml.org for more
 * information about SBML, and the latest version of libSBML.
 *
 * Copyright (C) 2009-2013 jointly by the following organizations: 
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
 * ---------------------------------------------------------------------- -->*/

#ifndef AddingConstraintsToValidator

#include <sbml/validator/VConstraint.h>
#include <sbml/packages/comp/validator/CompSBMLError.h>
#include <sbml/packages/comp/util/SBMLResolverRegistry.h>
#include <sbml/packages/comp/util/SBMLUri.h>
#include <sbml/SBMLTypes.h>
#include <sbml/packages/comp/common/CompExtensionTypes.h>
#include <sbml/util/ElementFilter.h>

#include "ExtModelReferenceCycles.h"
#include "SubmodelReferenceCycles.h"

#endif

#include <sbml/validator/ConstraintMacros.h>

/** @cond doxygen-ignored */

using namespace std;

/** @endcond */

/** 
 * This class implements an element filter, that can be used to find elements
 * with an id set
 */ 
class IdFilter : public ElementFilter
{
public:
	IdFilter() : ElementFilter()
	{
	}

	virtual bool filter(const SBase* element)
	{
		// return in case we don't have a valid element with an id
        if (element == NULL || element->isSetId() == false)
        {
            return false;
        }

        // otherwise we have an id set and want to keep the element
        // unless it is a rule or intialAssignment/eventAssignment
        int tc = element->getTypeCode();
        if (tc == SBML_ASSIGNMENT_RULE || tc == SBML_RATE_RULE
          || tc == SBML_INITIAL_ASSIGNMENT || tc == SBML_EVENT_ASSIGNMENT)
        {
          return false;
        }


        return true;			
	}

};

/** 
 * This class implements an element filter, that can be used to find elements
 * with an metaid set
 */ 
class MetaIdFilter : public ElementFilter
{
public:
	MetaIdFilter() : ElementFilter()
	{
	}

	virtual bool filter(const SBase* element)
	{
		// return in case we don't have a valid element with an id
        if (element == NULL || element->isSetMetaId() == false)
        {
            return false;
        }

        return true;			
	}

};

class ReferencedModel
{
public:
  ReferencedModel(const Model & m, const Port & p)
  {
    referencedModel = 
      static_cast<const Model*>(p.getAncestorOfType(SBML_MODEL, "core"));
    if (referencedModel == NULL) 
    {
      referencedModel = static_cast<const Model*>
        (p.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
    }
  }

  ReferencedModel(const Model & m, const Deletion & d)
  {
    referencedModel = NULL;

    const Submodel * sub = static_cast<const Submodel*>
                        (d.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
    if (sub != NULL)
    {
      std::string modelId = sub->getModelRef();

      const SBMLDocument * doc = d.getSBMLDocument();
      if (doc != NULL)
      {
        CompSBMLDocumentPlugin * docPlug = 
          (CompSBMLDocumentPlugin*)(doc->getPlugin("comp"));
        if (docPlug != NULL)
        {
          referencedModel = docPlug->getModelDefinition(modelId);
          if (referencedModel == NULL)
          {
            // may be an external model
            ExternalModelDefinition * emd = 
                               docPlug->getExternalModelDefinition(modelId);
            pre (emd != NULL);

            string locationURI = doc->getLocationURI();
            string uri = emd->getSource();

            const SBMLResolverRegistry& registry = 
                                  SBMLResolverRegistry::getInstance();
            SBMLDocument *newDoc = registry.resolve(uri, locationURI);
            if (newDoc != NULL) referencedModel = newDoc->getModel();
          }
        }
      }
    }
  }

 
  
  ReferencedModel(const Model & m, const ReplacedElement & repE)
  {
    referencedModel = NULL;

    CompModelPlugin *plug = (CompModelPlugin*)(m.getPlugin("comp"));
    
    if ((plug != NULL) && (plug->getSubmodel(repE.getSubmodelRef()) != NULL))
    {
      std::string modelId = 
               (plug->getSubmodel(repE.getSubmodelRef()))->getModelRef();

      const SBMLDocument * doc = repE.getSBMLDocument();

      if (doc != NULL)
      {
        CompSBMLDocumentPlugin * docPlug = 
          (CompSBMLDocumentPlugin*)(doc->getPlugin("comp"));
      
        if (docPlug != NULL)
        {

          referencedModel = docPlug->getModelDefinition(modelId);
          if (referencedModel == NULL)
          {
            // may be an external model
            ExternalModelDefinition * emd = 
                              docPlug->getExternalModelDefinition(modelId);
            pre (emd != NULL);

            string locationURI = doc->getLocationURI();
            string uri = emd->getSource();

            const SBMLResolverRegistry& registry = 
                                 SBMLResolverRegistry::getInstance();
            SBMLDocument *newDoc = registry.resolve(uri, locationURI);
            if (newDoc != NULL) referencedModel = newDoc->getModel();
          }
        }
      }
    }
  }

 
  
  ReferencedModel(const Model & m, const ReplacedBy & repBy)
  {
    referencedModel = NULL;

    CompModelPlugin *plug = (CompModelPlugin*)(m.getPlugin("comp"));
    
    if ((plug != NULL) && (plug->getSubmodel(repBy.getSubmodelRef()) != NULL))
    {
      std::string modelId = 
               (plug->getSubmodel(repBy.getSubmodelRef()))->getModelRef();

      const SBMLDocument * doc = repBy.getSBMLDocument();

      if (doc != NULL)
      {
        CompSBMLDocumentPlugin * docPlug = 
          (CompSBMLDocumentPlugin*)(doc->getPlugin("comp"));
      
        if (docPlug != NULL)
        {

          referencedModel = docPlug->getModelDefinition(modelId);
          if (referencedModel == NULL)
          {
            // may be an external model
            ExternalModelDefinition * emd = 
                              docPlug->getExternalModelDefinition(modelId);
            pre (emd != NULL);

            string locationURI = doc->getLocationURI();
            string uri = emd->getSource();

            const SBMLResolverRegistry& registry = 
                                 SBMLResolverRegistry::getInstance();
            SBMLDocument *newDoc = registry.resolve(uri, locationURI);
            if (newDoc != NULL) referencedModel = newDoc->getModel();
          }
        }
      }
    }
  }

 
  
  ReferencedModel(const Model & m, const SBaseRef & sbRef)
  {
    referencedModel = NULL;
  }


  const Model * getReferencedModel()
  {
    return referencedModel;
  }
private:

  const Model* referencedModel;
};

//*************************************

//SBase  constraints

// 20101 - caught at read
// 20102 - caught at read
// 20103 - caught at read
// 20104 - caught at read
// 20105 - caught at read

//*************************************

//SBML class  constraints

// 20201 - caught at read
// 20202 - caught at read
// 20203 - caught by checkConsistency
// 20204 - caught by checkConsistency
// 20205 - caught at read
// 20206 - caught at read
// 20207 - caught at read
// 20208 - caught at read
// 20209 - caught at read
// 20210 - caught at read
// 20211 - caught at read

//*************************************

//ExternalModelDefinition  constraints

// 20301 - caught at read
// 20302 - caught at read
// 20303 - caught at read

//20304
START_CONSTRAINT (CompReferenceMustBeL3, ExternalModelDefinition, emd)
{
  pre (emd.isSetSource() == true);
  pre (emd.isSetId() == true);
 
  bool fail = false;

  msg = "<externalModelDefinition> '";
  msg += emd.getId();
  msg += "' refers to a URI '";
  msg += emd.getSource();
  msg += "' which is not an SBML Level 3 document.";

  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
  const SBMLDocument* doc = emd.getSBMLDocument();
  pre(doc != NULL);
  string locationURI = doc->getLocationURI();
  string uri = emd.getSource();
  doc = NULL;

  doc = registry.resolve(uri, locationURI);
  pre (doc != NULL);

  if (doc->getLevel() != 3) 
  {
    fail = true;
  }

  delete doc;
  
  inv(fail == false);
}
END_CONSTRAINT

//20305
START_CONSTRAINT (CompModReferenceMustIdOfModel, ExternalModelDefinition, emd)
{
  pre (emd.isSetSource() == true);
  pre (emd.isSetId() == true);
  pre (emd.isSetModelRef() == true);
  
  bool fail = false;

  msg = "<externalModelDefinition> '";
  msg += emd.getId() ;
  msg += "' refers to a model with id '";
  msg += emd.getModelRef();
  msg += "' that does not exist in the referenced document.";

  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
  const SBMLDocument* doc = emd.getSBMLDocument();
  pre(doc != NULL);
  string locationURI = doc->getLocationURI();
  string uri = emd.getSource();

  //SBMLUri* resolved = registry.resolveUri(uri, locationURI);
  //pre(resolved != NULL )
  //string resolvedURI = resolved->getUri();
  //delete resolved;
  doc = registry.resolve(uri, locationURI);
  pre(doc != NULL);
  pre(doc->getLevel() == 3);

  const CompSBMLDocumentPlugin* csdp = 
    static_cast<const CompSBMLDocumentPlugin*>(doc->getPlugin("comp"));
  if (csdp == NULL) 
  {
    const Model* model = doc->getModel();
    if (model==NULL || (model->getId() != emd.getModelRef())) {
      fail = true;
    }
  }
  else 
  {
    const SBase* referencedmod = csdp->getModel(emd.getModelRef());
    if (referencedmod == NULL) 
    {
      fail = true;
    }
  }

  delete doc;
  inv(fail == false);
}
END_CONSTRAINT

//TODO: 20306 - caught at read md5
// 20307 - caught at read anyURI
// 20308 - caught at read
// 20309 - string
// 20310 
EXTERN_CONSTRAINT( CompCircularExternalModelReference, ExtModelReferenceCycles)

//90101
START_CONSTRAINT (CompUnresolvedReference, ExternalModelDefinition, emd)
{
  pre (emd.isSetSource() == true);
  
  const SBMLDocument* doc = emd.getSBMLDocument();
  pre(doc != NULL);
  string locationURI = doc->getLocationURI();
  string uri = emd.getSource();

  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
  SBMLUri* resolved = registry.resolveUri(uri, locationURI);

  bool fail = false;

  msg = "<externalModelDefinition> '";
  msg += emd.getId() ;
  msg += "' refers to a source '";
  msg += emd.getSource();
  msg += "' that cannot be accessed from here. Further checks relating to.";
  msg += "' this document cannot be performed.";

  if (resolved == NULL) 
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT


//*************************************

// 204xx - not used 

//*************************************

//Model  constraints

//20501 - caught at read
//20502 - caught at read
//20503 - caught at read
//20504 - caught at read
//20505 - caught at read
//20506 - caught at read


//*************************************

//Submodel constraints

//20601 - caught at read
//20602 - caught at read
//20603 - caught at read
//20604 - caught at read
//20605 - caught at read
//20606 - caught at read
//20607 - caught at read
//20608 - caught at read

//20609-20612 - non existant

//20613 - caught at read
//20614 - caught at read

//20615
START_CONSTRAINT (CompSubmodelMustReferenceModel, Submodel, s)
{
  pre (s.isSetModelRef() == true);

  bool fail = true;

  msg = "<submodel> '";
  msg += s.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                     (s.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                      (s.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to a model with id '";
  msg += s.getModelRef();
  msg += "' that does not exist in the referenced document.";

  // do we reference the actual model
  // do not report this here as it is another error
  if (s.getModelRef() == m.getId())
  {
    fail = false;
  }

  if (fail == true)
  {
    // do we refernce an external modelDefinition
    CompSBMLDocumentPlugin *docPlug = (CompSBMLDocumentPlugin*)
      (m.getSBMLDocument()->getPlugin("comp"));
    pre (docPlug != NULL);

    ModelDefinition * md = docPlug->getModelDefinition(s.getModelRef());
    if (md == NULL)
    {
      ExternalModelDefinition * ext = 
        docPlug->getExternalModelDefinition(s.getModelRef());

      if (ext != NULL)
      {
        fail = false;
      }
    }
    else
    {
      fail = false;
    }
  }

  inv(fail == false);
}
END_CONSTRAINT

//20616
START_CONSTRAINT (CompSubmodelCannotReferenceSelf, Submodel, s)
{
  pre (s.isSetModelRef() == true);

  bool fail = false;

  msg = "<submodel> '";
  msg += s.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                (s.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) 
  {
    mod = static_cast<const Model*>
                      (s.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to the enclosing model with id '";
  msg += s.getModelRef();
  msg += "'.";

  if (m.getId() == s.getModelRef())
  {
    fail = true;
  }
  
  inv(fail == false);

}
END_CONSTRAINT

// 20617 
EXTERN_CONSTRAINT( CompModCannotCircularlyReferenceSelf, 
                                                SubmodelReferenceCycles)

// 20618 - 20621 non existant

//20622
START_CONSTRAINT (CompTimeConversionMustBeParameter, Submodel, s)
{
  pre (s.isSetTimeConversionFactor() == true);

  bool fail = false;

  msg = "The 'timeConversionFactor' of <submodel> '";
  msg += s.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                (s.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                     (s.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " is set to '";
  msg += s.getTimeConversionFactor();
  msg += "' which is not a <parameter> within the <model>.";

  if (m.getParameter(s.getTimeConversionFactor()) == NULL)
  {
    fail = true;
  }
  
  inv(fail == false);

}
END_CONSTRAINT

//20623
START_CONSTRAINT (CompExtentConversionMustBeParameter, Submodel, s)
{
  pre (s.isSetExtentConversionFactor() == true);

  bool fail = false;

  msg = "The 'extentConversionFactor' of <submodel> '";
  msg += s.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                (s.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                      (s.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " is set to '";
  msg += s.getExtentConversionFactor();
  msg += "' which is not a <parameter> within the <model>.";

  if (m.getParameter(s.getExtentConversionFactor()) == NULL)
  {
    fail = true;
  }
  
  inv(fail == false);

}
END_CONSTRAINT

//*************************************

//SBaseRef constraints
// -  need to implement for each object that derives from SBaseRef
// Port; Deletion; ReplacedElement; ReplacedBy

//20701
// Port doesnt have portRef

// 20701 - deletion
START_CONSTRAINT (CompPortRefMustReferencePort, Deletion, d)
{
  pre(d.isSetPortRef());
  
  bool fail = false;

  const Submodel * sub = static_cast<const Submodel*>
                        (d.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
  pre (sub != NULL);

  msg = "The 'portRef' of a <deletion>";
  msg += " is set to '";
  msg += d.getPortRef();
  msg += "' which is not a <port> within the <model> referenced by ";
  msg += "submodel '";
  msg += sub->getId();
  msg += "'.";

  ReferencedModel *ref = new ReferencedModel(m, d);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  CompModelPlugin *plug1 = 
                  (CompModelPlugin*)(referencedModel->getPlugin("comp"));
  pre (plug1 != NULL);

  if (plug1->getPort(d.getPortRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20701 - replacedElement
START_CONSTRAINT (CompPortRefMustReferencePort, ReplacedElement, repE)
{
  pre(repE.isSetPortRef());
  pre(repE.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'portRef' of a <replacedElement>";
  msg += " is set to '";
  msg += repE.getPortRef();
  msg += "' which is not a <port> within the <model> referenced by ";
  msg += "submodel '";
  msg += repE.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repE);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  CompModelPlugin *plug1 = 
                   (CompModelPlugin*)(referencedModel->getPlugin("comp"));
  pre (plug1 != NULL);

  if (plug1->getPort(repE.getPortRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20701 - replacedBy
START_CONSTRAINT (CompPortRefMustReferencePort, ReplacedBy, repBy)
{
  pre(repBy.isSetPortRef());
  pre(repBy.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'portRef' of a <replacedBy>";
  msg += " is set to '";
  msg += repBy.getPortRef();
  msg += "' which is not a <port> within the <model> referenced by ";
  msg += "submodel '";
  msg += repBy.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repBy);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  CompModelPlugin *plug1 = 
                  (CompModelPlugin*)(referencedModel->getPlugin("comp"));
  pre (plug1 != NULL);

  if (plug1->getPort(repBy.getPortRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20702
//20702 - port
START_CONSTRAINT (CompIdRefMustReferenceObject, Port, p)
{
  pre(p.isSetIdRef());
  
  bool fail = false;

  msg = "The 'idRef' of a <port>";
  msg += " is set to '";
  msg += p.getIdRef();
  msg += "' which is not an element within the <model>.";

  IdList mIds;

  // create the filter we want to use
  IdFilter filter;

  ReferencedModel *ref = new ReferencedModel(m, p);
  const Model* mod = ref->getReferencedModel();
  
  pre (mod != NULL);
  
  List* allElements = const_cast<Model*>(mod)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getId());
  }


  if (mIds.contains(p.getIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20702 - deletion
START_CONSTRAINT (CompIdRefMustReferenceObject, Deletion, d)
{
  pre(d.isSetIdRef());
  
  bool fail = false;

  const Submodel * sub = static_cast<const Submodel*>
                        (d.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
  pre (sub != NULL);

  msg = "The 'idRef' of a <deletion>";
  msg += " is set to '";
  msg += d.getIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += sub->getId();
  msg += "'.";

  ReferencedModel *ref = new ReferencedModel(m, d);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  IdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                                (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getId());
  }


  if (mIds.contains(d.getIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20702 - replacedElement
START_CONSTRAINT (CompIdRefMustReferenceObject, ReplacedElement, repE)
{
  pre(repE.isSetIdRef());
  pre(repE.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'idRef' of a <replacedElement>";
  msg += " is set to '";
  msg += repE.getIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += repE.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repE);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  IdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                               (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getId());
  }


  if (mIds.contains(repE.getIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20702 - replacedBy
START_CONSTRAINT (CompIdRefMustReferenceObject, ReplacedBy, repBy)
{
  pre(repBy.isSetIdRef());
  pre(repBy.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'idRef' of a <replacedBy>";
  msg += " is set to '";
  msg += repBy.getIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += repBy.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repBy);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  IdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                               (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getId());
  }


  if (mIds.contains(repBy.getIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20702 - sBaseRef
START_CONSTRAINT (CompIdRefMustReferenceObject, SBaseRef, sbRef)
{
  pre(sbRef.isSetIdRef());

  bool fail = false;

  bool parentModelFound = false;

  pre (sbRef.getParentSBMLObject() != NULL);

  std::string subModelRef;
  std::string idRef;
  std::string metaIdRef;
  int tc = sbRef.getParentSBMLObject()->getTypeCode();
      
  const ReplacedElement * repE = NULL;
  const ReplacedBy* repBy = NULL;
  const Deletion * del = NULL;
  const Port * port = NULL;
  const SBaseRef * parent = NULL;

  switch (tc)
  {
    case SBML_COMP_REPLACEDELEMENT:
      repE = 
            static_cast<const ReplacedElement*>(sbRef.getParentSBMLObject());
      pre (repE != NULL);
      pre (repE->isSetIdRef() == true || repE->isSetMetaIdRef() == true);
      subModelRef = repE->getSubmodelRef();
      idRef = repE->getIdRef();
      metaIdRef = repE->getMetaIdRef();
      break;
    case SBML_COMP_REPLACEDBY:
      repBy = static_cast<const ReplacedBy*>(sbRef.getParentSBMLObject());
      pre (repBy != NULL);
      pre (repBy->isSetIdRef() == true || repBy->isSetMetaIdRef() == true);
      subModelRef = repBy->getSubmodelRef();
      idRef = repBy->getIdRef();
      metaIdRef = repBy->getMetaIdRef();
      break;
    case SBML_COMP_PORT:
      port = static_cast<const Port*>(sbRef.getParentSBMLObject());
      pre (port != NULL);
      pre (port->isSetIdRef() == true || port->isSetMetaIdRef() == true);
      idRef = port->getIdRef();
      metaIdRef = port->getMetaIdRef();
     break;
    case SBML_COMP_DELETION:
      del = static_cast<const Deletion*>(sbRef.getParentSBMLObject());
      pre (del != NULL);
      pre (del->isSetIdRef() == true || del->isSetMetaIdRef() == true);
      idRef = del->getIdRef();
      metaIdRef = del->getMetaIdRef();
     break;
    case SBML_COMP_SBASEREF:
      parent = static_cast<const SBaseRef*>(sbRef.getParentSBMLObject());
      pre (parent != NULL);
      pre (parent->isSetIdRef() == true || parent->isSetMetaIdRef() == true);
      idRef = parent->getIdRef();
      metaIdRef = parent->getMetaIdRef();
     break;


  }


  /* need to be using the correct model */
  CompModelPlugin *plug = (CompModelPlugin*)(m.getPlugin("comp"));
  
  pre (plug != NULL);

  std::string modelId;
  Model * referencedModel = NULL;
  const Model* mod = NULL;
  const SBMLDocument * doc = sbRef.getSBMLDocument();
  pre (doc != NULL);

  CompSBMLDocumentPlugin * docPlug = 
                          (CompSBMLDocumentPlugin*)(doc->getPlugin("comp"));
  pre (docPlug != NULL); 
  
  if (tc == SBML_COMP_REPLACEDELEMENT || tc == SBML_COMP_REPLACEDBY)
  {
    msg = "The 'idRef' of a <sBaseRef>";
    msg += " is set to '";
    msg += sbRef.getIdRef();
    msg += "' which is not an element within the <model> referenced by ";
    msg += "submodel '";
    msg += subModelRef;
    msg += "'.";

    pre (subModelRef.empty() == false);
    pre (plug->getSubmodel(subModelRef) != NULL);

    modelId = (plug->getSubmodel(subModelRef))->getModelRef();

    //referencedModel = docPlug->getModelDefinition(modelId);
    //if (referencedModel == NULL)
    //{
    //  // may be an external model
    //  ExternalModelDefinition * emd = 
    //                            docPlug->getExternalModelDefinition(modelId);
    //  pre (emd != NULL);

    //  string locationURI = doc->getLocationURI();
    //  string uri = emd->getSource();

    //  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
    //  SBMLDocument *newDoc = registry.resolve(uri, locationURI);
    //  pre(newDoc != NULL);
    //  referencedModel = newDoc->getModel();
    //}
  }
  else if (tc == SBML_COMP_PORT)
  {
    msg = "The 'idRef' of a <sBaseRef>";
    msg += " is set to '";
    msg += sbRef.getIdRef();
    msg += "' which is not an element within the <model> referenced by ";
    msg += "port '";
    msg += port->getId();
    msg += "'.";

    mod = static_cast<const Model*>
                      (port->getAncestorOfType(SBML_MODEL, "core"));
    if (mod == NULL) 
    {
      mod = static_cast<const Model*>
        (port->getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
    }
    referencedModel = const_cast<Model*>(mod);
    parentModelFound = true;
  }
  else if (tc == SBML_COMP_DELETION)
  {
    const Submodel * sub = static_cast<const Submodel*>
                           (del->getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));

    pre (sub != NULL);

    msg = "The 'idRef' of a <deletion>";
    msg += " is set to '";
    msg += sbRef.getIdRef();
    msg += "' which is not an element within the <model> referenced by ";
    msg += "the submodel '";
    msg += sub->getId();
    msg += "'.";

    modelId = sub->getModelRef();

    //referencedModel = docPlug->getModelDefinition(modelId);
    //if (referencedModel == NULL)
    //{
    //  // may be an external model
    //  ExternalModelDefinition * emd = 
    //                            docPlug->getExternalModelDefinition(modelId);
    //  pre (emd != NULL);

    //  string locationURI = doc->getLocationURI();
    //  string uri = emd->getSource();

    //  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
    //  SBMLDocument *newDoc = registry.resolve(uri, locationURI);
    //  pre(newDoc != NULL);
    //  referencedModel = newDoc->getModel();
    //}
  }
  else if (tc == SBML_COMP_SBASEREF)
  {
    msg = "The 'idRef' of a <sBaseRef>";
    msg += " is set to '";
    msg += sbRef.getIdRef();
    msg += "' which is not an element within the <model> referenced by ";
    msg += "the parent sBaseRef.";

    pre (parent->getParentSBMLObject() != NULL);

    int tc1 = parent->getParentSBMLObject()->getTypeCode();
    const SBase * grandparent = parent->getParentSBMLObject();
    while (tc1 == SBML_COMP_SBASEREF)
    {
      grandparent = grandparent->getParentSBMLObject();
      tc1 = grandparent->getTypeCode();
    }
    switch (tc1)
    {
    case SBML_COMP_REPLACEDELEMENT:
      pre (static_cast<const ReplacedElement*>(grandparent)
                             ->getSubmodelRef().empty() == false);
      modelId = (plug->getSubmodel(static_cast<const ReplacedElement*>(grandparent)
                             ->getSubmodelRef()))->getModelRef();
      break;
    case SBML_COMP_REPLACEDBY:
      pre (static_cast<const ReplacedBy*>(grandparent)
                             ->getSubmodelRef().empty() == false);
      modelId = (plug->getSubmodel(static_cast<const ReplacedBy*>(grandparent)
                             ->getSubmodelRef()))->getModelRef();
      break;
    case SBML_COMP_PORT:
      mod = static_cast<const Model*>
                        (grandparent->getAncestorOfType(SBML_MODEL, "core"));
      if (mod == NULL) 
      {
        mod = static_cast<const Model*>
          (grandparent->getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
      }
      referencedModel = const_cast<Model*>(mod);
      parentModelFound = true;
      break;

    }

    //referencedModel = docPlug->getModelDefinition(modelId);
    //if (referencedModel == NULL)
    //{
    //  // may be an external model
    //  ExternalModelDefinition * emd = 
    //                            docPlug->getExternalModelDefinition(modelId);
    //  pre (emd != NULL);

    //  string locationURI = doc->getLocationURI();
    //  string uri = emd->getSource();

    //  const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
    //  SBMLDocument *newDoc = registry.resolve(uri, locationURI);
    //  pre(newDoc != NULL);
    //  referencedModel = newDoc->getModel();
    //}
  }

  if (parentModelFound == false)
  {
    pre (modelId.empty() == false);

    referencedModel = docPlug->getModelDefinition(modelId);
    if (referencedModel == NULL)
    {
      // may be an external model
      ExternalModelDefinition * emd = 
                                docPlug->getExternalModelDefinition(modelId);
      pre (emd != NULL);

      string locationURI = doc->getLocationURI();
      string uri = emd->getSource();

      const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
      SBMLDocument *newDoc = registry.resolve(uri, locationURI);
      pre(newDoc != NULL);
      referencedModel = newDoc->getModel();
    }
  }
  
  /* so this is the submodel pointed to by the parent 
   * but the parent will have an idRef or metaidRef that points to
   * a submodel within this model
   */
  
  pre (referencedModel != NULL);

  CompModelPlugin *plug1 = 
                  (CompModelPlugin*)(referencedModel->getPlugin("comp"));
  pre (plug1 != NULL);

  if (idRef.empty() == false)
  {
    pre (plug1->getSubmodel(idRef) != NULL);

    modelId = (plug1->getSubmodel(idRef))->getModelRef();
  }
  else
  {
    for (unsigned int i = 0; i < plug1->getNumSubmodels(); i++)
    {
      if (plug1->getSubmodel(i)->getMetaId() == metaIdRef)
      {
        modelId = plug1->getSubmodel(i)->getModelRef();
        break;
      }
    }
  }

  referencedModel = docPlug->getModelDefinition(modelId);
  if (referencedModel == NULL)
  {
    // may be an external model
    ExternalModelDefinition * emd = 
                              docPlug->getExternalModelDefinition(modelId);
    pre (emd != NULL);

    string locationURI = doc->getLocationURI();
    string uri = emd->getSource();

    const SBMLResolverRegistry& registry = SBMLResolverRegistry::getInstance();
    SBMLDocument *newDoc = registry.resolve(uri, locationURI);
    pre(newDoc != NULL);
    referencedModel = newDoc->getModel();
  }

  pre (referencedModel != NULL);


  IdList mIds;

  // create the filter we want to use
  IdFilter filter;

  //  get a list of all elements with an id
  List* allElements = referencedModel->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getId());
  }


  if (mIds.contains(sbRef.getIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20703
//20703 - port
START_CONSTRAINT (CompUnitRefMustReferenceUnitDef, Port, p)
{
  pre(p.isSetUnitRef());
  
  bool fail = false;

  msg = "The 'unitRef' of a <port>";
  msg += " is set to '";
  msg += p.getUnitRef();
  msg += "' which is not a <unitDefinition> within the <model>.";

  if (m.getUnitDefinition(p.getUnitRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20703 - deletion
START_CONSTRAINT (CompUnitRefMustReferenceUnitDef, Deletion, d)
{
  pre(d.isSetUnitRef());
  
  bool fail = false;

  const Submodel * sub = static_cast<const Submodel*>
                         (d.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
  pre (sub != NULL);

  msg = "The 'unitRef' of a <deletion>";
  msg += " is set to '";
  msg += d.getUnitRef();
  msg += "' which is not a <unitDefinition> within the <model> referenced by ";
  msg += "submodel '";
  msg += sub->getId();
  msg += "'.";

  ReferencedModel *ref = new ReferencedModel(m, d);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  if (referencedModel->getUnitDefinition(d.getUnitRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20703 - replacedElement
START_CONSTRAINT (CompUnitRefMustReferenceUnitDef, ReplacedElement, repE)
{
  pre(repE.isSetUnitRef());
  pre(repE.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'unitRef' of a <replacedElement>";
  msg += " is set to '";
  msg += repE.getUnitRef();
  msg += "' which is not a <unitDefinition> within the <model> referenced by ";
  msg += "submodel '";
  msg += repE.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repE);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  if (referencedModel->getUnitDefinition(repE.getUnitRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20703 - replacedBy
START_CONSTRAINT (CompUnitRefMustReferenceUnitDef, ReplacedBy, repBy)
{
  pre(repBy.isSetUnitRef());
  pre(repBy.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'unitRef' of a <replacedBy>";
  msg += " is set to '";
  msg += repBy.getUnitRef();
  msg += "' which is not a <unitDefinition> within the <model> referenced by ";
  msg += "submodel '";
  msg += repBy.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repBy);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  if (referencedModel->getUnitDefinition(repBy.getUnitRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20704
//20704 - port
START_CONSTRAINT (CompMetaIdRefMustReferenceObject, Port, p)
{
  pre(p.isSetMetaIdRef());
  
  bool fail = false;

  msg = "The 'metaIdRef' of a <port>";
  msg += " is set to '";
  msg += p.getMetaIdRef();
  msg += "' which is not an element within the <model>.";

  IdList mIds;

  // create the filter we want to use
  MetaIdFilter filter;

  //  get a list of all elements with an id
  ReferencedModel *ref = new ReferencedModel(m, p);
  const Model* mod = ref->getReferencedModel();

  pre (mod != NULL);
  
  List* allElements = const_cast<Model*>(mod)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getMetaId());
  }


  if (mIds.contains(p.getMetaIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20704 - deletion
START_CONSTRAINT (CompMetaIdRefMustReferenceObject, Deletion, d)
{
  pre(d.isSetMetaIdRef());
  
  bool fail = false;

  const Submodel * sub = static_cast<const Submodel*>
                        (d.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
  pre (sub != NULL);

  msg = "The 'metaIdRef' of a <deletion>";
  msg += " is set to '";
  msg += d.getMetaIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += sub->getId();
  msg += "'.";

  ReferencedModel *ref = new ReferencedModel(m, d);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  MetaIdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                                (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getMetaId());
  }


  if (mIds.contains(d.getMetaIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20704 - replacedElement
START_CONSTRAINT (CompMetaIdRefMustReferenceObject, ReplacedElement, repE)
{
  pre(repE.isSetMetaIdRef());
  pre(repE.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'metaidRef' of a <replacedElement>";
  msg += " is set to '";
  msg += repE.getMetaIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += repE.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repE);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  MetaIdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                               (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getMetaId());
  }


  if (mIds.contains(repE.getMetaIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20704 - replacedBy
START_CONSTRAINT (CompMetaIdRefMustReferenceObject, ReplacedBy, repBy)
{
  pre(repBy.isSetMetaIdRef());
  pre(repBy.isSetSubmodelRef());

  bool fail = false;

  msg = "The 'metaIdRef' of a <replacedBy>";
  msg += " is set to '";
  msg += repBy.getMetaIdRef();
  msg += "' which is not an element within the <model> referenced by ";
  msg += "submodel '";
  msg += repBy.getSubmodelRef();
  msg += "'.";

  /* need to be using the correct model */
  ReferencedModel *ref = new ReferencedModel(m, repBy);
  const Model* referencedModel = ref->getReferencedModel();

  pre (referencedModel != NULL);

  IdList mIds;

  // create the filter we want to use
  MetaIdFilter filter;

  //  get a list of all elements with an id
  List* allElements = const_cast<Model*>
                                (referencedModel)->getAllElements(&filter);

  for (unsigned int i = 0; i < allElements->getSize(); i++)
  {
    mIds.append(static_cast<SBase*>(allElements->get(i))->getMetaId());
  }


  if (mIds.contains(repBy.getMetaIdRef()) == false)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT


// 20705
// 20705 - port
START_CONSTRAINT (CompParentOfSBRefChildMustBeSubmodel, Port, port)
{
  pre (port.isSetSBaseRef());

  bool fail = false;

  if (port.isSetIdRef() == true || port.isSetMetaIdRef() == true)
  {
    if (port.isSetIdRef() == true)
    {
      msg = "The 'idRef' of a <replacedElement>";
      msg += " is set to '";
      msg += port.getIdRef();
    }
    else
    {
      msg = "The 'metaIdRef' of a <replacedElement>";
      msg += " is set to '";
      msg += port.getMetaIdRef();
    }
    msg += "' which is not a submodel within the <model>.";

    /* need to be using the correct model */
    ReferencedModel *ref = new ReferencedModel(m, port);
    const Model* mod = ref->getReferencedModel();

    pre (mod != NULL);

    CompModelPlugin *plug = (CompModelPlugin*)(mod->getPlugin("comp"));
    
    pre (plug != NULL);

    if (port.isSetIdRef() == true)
    {
      if (plug->getSubmodel(port.getIdRef()) == NULL)
      {
        fail = true;
      }
    }
    else
    {
      // must be a metaidref
      std::string ref = port.getMetaIdRef();
      bool found = false;
      unsigned int i = 0;
      while (found == false &&  i < plug->getNumSubmodels())
      {
        if (ref == plug->getSubmodel(i)->getMetaId())
        {
          found = true;
        }

        i++;
      }
      if (found == false)
      {
        fail = true;
      }
    }
  }
  else
  {
    fail = true;

    if (port.isSetUnitRef() == true)
    {
      msg = "The 'unitRef' of a <replacedElement>";
      msg += " is set to '";
      msg += port.getUnitRef();
    }
    msg += "' which is not a submodel within the <model>.";
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20705 - deletion
START_CONSTRAINT (CompParentOfSBRefChildMustBeSubmodel, Deletion, del)
{
  pre (del.isSetSBaseRef());

  bool fail = false;

  const Submodel * sub = static_cast<const Submodel*>
                         (del.getAncestorOfType(SBML_COMP_SUBMODEL, "comp"));
  pre (sub != NULL);

  if (del.isSetIdRef() == true || del.isSetMetaIdRef() == true)
  {
    if (del.isSetIdRef() == true)
    {
      msg = "The 'idRef' of a <deletion>";
      msg += " is set to '";
      msg += del.getIdRef();
    }
    else
    {
      msg = "The 'metaIdRef' of a <deletion>";
      msg += " is set to '";
      msg += del.getMetaIdRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += sub->getId();
    msg += "'.";

    /* need to be using the correct model */
    ReferencedModel *ref = new ReferencedModel(m, del);
    const Model* referencedModel = ref->getReferencedModel();

    pre (referencedModel != NULL);

    CompModelPlugin *plug1 = 
                    (CompModelPlugin*)(referencedModel->getPlugin("comp"));
    pre (plug1 != NULL);

    if (del.isSetIdRef() == true)
    {
      if (plug1->getSubmodel(del.getIdRef()) == NULL)
      {
        fail = true;
      }
    }
    else
    {
      // must be a metaidref
      std::string ref = del.getMetaIdRef();
      bool found = false;
      unsigned int i = 0;
      while (found == false &&  i < plug1->getNumSubmodels())
      {
        if (ref == plug1->getSubmodel(i)->getMetaId())
        {
          found = true;
        }

        i++;
      }
      if (found == false)
      {
        fail = true;
      }
    }
  }
  else
  {
    fail = true;

    if (del.isSetPortRef() == true)
    {
      msg = "The 'portRef' of a <deletion>";
      msg += " is set to '";
      msg += del.getPortRef();
    }
    else
    {
      msg = "The 'unitRef' of a <deletion>";
      msg += " is set to '";
      msg += del.getUnitRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += sub->getId();
    msg += "'.";  
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20705 - replacedElement
START_CONSTRAINT (CompParentOfSBRefChildMustBeSubmodel, ReplacedElement, repE)
{
  pre (repE.isSetSBaseRef());

  bool fail = false;

  if (repE.isSetIdRef() == true || repE.isSetMetaIdRef() == true)
  {
    if (repE.isSetIdRef() == true)
    {
      msg = "The 'idRef' of a <replacedElement>";
      msg += " is set to '";
      msg += repE.getIdRef();
    }
    else
    {
      msg = "The 'metaIdRef' of a <replacedElement>";
      msg += " is set to '";
      msg += repE.getMetaIdRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += repE.getSubmodelRef();
    msg += "'.";

    /* need to be using the correct model */
    ReferencedModel *ref = new ReferencedModel(m, repE);
    const Model* referencedModel = ref->getReferencedModel();

    pre (referencedModel != NULL);

    CompModelPlugin *plug1 = 
                    (CompModelPlugin*)(referencedModel->getPlugin("comp"));
    pre (plug1 != NULL);

    if (repE.isSetIdRef() == true)
    {
      if (plug1->getSubmodel(repE.getIdRef()) == NULL)
      {
        fail = true;
      }
    }
    else
    {
      // must be a metaidref
      std::string ref = repE.getMetaIdRef();
      bool found = false;
      unsigned int i = 0;
      while (found == false &&  i < plug1->getNumSubmodels())
      {
        if (ref == plug1->getSubmodel(i)->getMetaId())
        {
          found = true;
        }

        i++;
      }
      if (found == false)
      {
        fail = true;
      }
    }
  }
  else
  {
    fail = true;

    if (repE.isSetPortRef() == true)
    {
      msg = "The 'portRef' of a <replacedElement>";
      msg += " is set to '";
      msg += repE.getPortRef();
    }
    else
    {
      msg = "The 'unitRef' of a <replacedElement>";
      msg += " is set to '";
      msg += repE.getUnitRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += repE.getSubmodelRef();
    msg += "'.";  
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20705 - replacedBy
START_CONSTRAINT (CompParentOfSBRefChildMustBeSubmodel, ReplacedBy, repE)
{
  pre (repE.isSetSBaseRef());

  bool fail = false;

  if (repE.isSetIdRef() == true || repE.isSetMetaIdRef() == true)
  {
    if (repE.isSetIdRef() == true)
    {
      msg = "The 'idRef' of a <replacedBy>";
      msg += " is set to '";
      msg += repE.getIdRef();
    }
    else
    {
      msg = "The 'metaIdRef' of a <replacedBy>";
      msg += " is set to '";
      msg += repE.getMetaIdRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += repE.getSubmodelRef();
    msg += "'.";

    /* need to be using the correct model */
    ReferencedModel *ref = new ReferencedModel(m, repE);
    const Model* referencedModel = ref->getReferencedModel();

    pre (referencedModel != NULL);

    CompModelPlugin *plug1 = 
                    (CompModelPlugin*)(referencedModel->getPlugin("comp"));
    pre (plug1 != NULL);

    if (repE.isSetIdRef() == true)
    {
      if (plug1->getSubmodel(repE.getIdRef()) == NULL)
      {
        fail = true;
      }
    }
    else
    {
      // must be a metaidref
      std::string ref = repE.getMetaIdRef();
      bool found = false;
      unsigned int i = 0;
      while (found == false &&  i < plug1->getNumSubmodels())
      {
        if (ref == plug1->getSubmodel(i)->getMetaId())
        {
          found = true;
        }

        i++;
      }
      if (found == false)
      {
        fail = true;
      }
    }
  }
  else
  {
    fail = true;

    if (repE.isSetPortRef() == true)
    {
      msg = "The 'portRef' of a <replacedBy>";
      msg += " is set to '";
      msg += repE.getPortRef();
    }
    else
    {
      msg = "The 'unitRef' of a <replacedBy>";
      msg += " is set to '";
      msg += repE.getUnitRef();
    }
    msg += "' which is not a submodel within the <model> referenced by ";
    msg += "submodel '";
    msg += repE.getSubmodelRef();
    msg += "'.";  
  }

  inv(fail == false);
}
END_CONSTRAINT

// 20705 - sBaseRef
START_CONSTRAINT (CompParentOfSBRefChildMustBeSubmodel, SBaseRef, sbRef)
{
  // TO DO deal with nested SBaseRefs
  pre (sbRef.isSetSBaseRef());

  bool fail = false;

  if (sbRef.isSetIdRef() == true || sbRef.isSetMetaIdRef() == true)
  {
    if (sbRef.isSetIdRef() == true)
    {
      msg = "The 'idRef' of a <sBaseRef>";
      msg += " is set to '";
      msg += sbRef.getIdRef();
    }
    else
    {
      msg = "The 'metaIdRef' of a <sbaseRef>";
      msg += " is set to '";
      msg += sbRef.getMetaIdRef();
    }
    msg += "' which is not a submodel within the <model>.";

    /* need to be using the correct model */
    const Model* mod = 
      static_cast<const Model*>(sbRef.getAncestorOfType(SBML_MODEL, "core"));
    if (mod == NULL) 
    {
      mod = static_cast<const Model*>
        (sbRef.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
    }
    CompModelPlugin *plug = (CompModelPlugin*)(mod->getPlugin("comp"));
    
    pre (plug != NULL);

    if (sbRef.isSetIdRef() == true)
    {
      if (plug->getSubmodel(sbRef.getIdRef()) == NULL)
      {
        fail = true;
      }
    }
    else
    {
      // must be a metaidref
      std::string ref = sbRef.getMetaIdRef();
      bool found = false;
      unsigned int i = 0;
      while (found == false &&  i < plug->getNumSubmodels())
      {
        if (ref == plug->getSubmodel(i)->getMetaId())
        {
          found = true;
        }

        i++;
      }
      if (found == false)
      {
        fail = true;
      }
    }
  }
  else
  {
    fail = true;

    if (sbRef.isSetUnitRef() == true)
    {
      msg = "The 'unitRef' of a <sBaseRef>";
      msg += " is set to '";
      msg += sbRef.getUnitRef();
    }
    msg += "' which is not a submodel within the <model>.";
  }

  inv(fail == false);
}
END_CONSTRAINT


//20706 - caught at read
//20707 - caught at read
//20708 - caught at read
//20709 - caught at read
//20710 - caught at read
//20711 - caught at read

//*************************************

//Port constraints

//20801
START_CONSTRAINT (CompPortMustReferenceObject, Port, p)
{
  pre (p.isSetId());

  bool idRef = p.isSetIdRef();
  bool unitRef = p.isSetUnitRef();
  bool metaidRef = p.isSetMetaIdRef();

  msg = "<port> '";
  msg += p.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                    (p.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                     (p.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " does not refer to another object.";

  bool fail = true;

  if (idRef == true)
  {
    fail = false;
  }
  else if (unitRef == true)
  {
    fail = false;
  }
  else if (metaidRef == true)
  {
    fail = false;
  }

  inv(fail == false);


}
END_CONSTRAINT


//20802
START_CONSTRAINT (CompPortMustReferenceOnlyOneObject, Port, p)
{
  pre (p.isSetId());

  bool idRef = p.isSetIdRef();
  bool unitRef = p.isSetUnitRef();
  bool metaidRef = p.isSetMetaIdRef();

  bool fail = false;

  msg = "<port> '";
  msg += p.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                    (p.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                     (p.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to ";

  if (idRef == true)
  {
    msg += "object with id '";
    msg += p.getIdRef();
    msg += "' and also ";
    if (unitRef == true)
    {
      fail = true;
      msg += "unit with id '";
      msg += p.getUnitRef();
      msg += "'";

      if ( metaidRef == true)
      {
        msg += "and also object with metaid '";
        msg += p.getMetaIdRef();
        msg += "'.";
      }
    }
    else if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += p.getMetaIdRef();
      msg += "'.";
    }
  }
  else if (unitRef == true)
  {
    msg += "unit with id '";
    msg += p.getUnitRef();
    msg += "' and also ";
    
    if (metaidRef == true)
    {
      fail = true;
      msg += "object with metaid '";
      msg += p.getMetaIdRef();
      msg += "'.";
    }
  }

  inv(fail == false);

}
END_CONSTRAINT


//20803 - caught at read

//TODO:20804 - to do

//*************************************

//Deletion constraints

//20901
START_CONSTRAINT (CompDeletionMustReferenceObject, Deletion, d)
{
  //pre (d.isSetId());

  bool idRef = d.isSetIdRef();
  bool unitRef = d.isSetUnitRef();
  bool metaidRef = d.isSetMetaIdRef();
  bool portRef  = d.isSetPortRef();

  msg = "<Deletion> '";
  msg += d.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                    (d.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                     (d.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " does not refer to another object.";

  bool fail = true;

  if (idRef == true)
  {
    fail = false;
  }
  else if (unitRef == true)
  {
    fail = false;
  }
  else if (metaidRef == true)
  {
    fail = false;
  }
  else if (portRef == true)
  {
    fail = false;
  }

  inv(fail == false);


}
END_CONSTRAINT


//20902
START_CONSTRAINT (CompDeletionMustReferOnlyOneObject, Deletion, d)
{
  //pre (d.isSetId());

  bool idRef = d.isSetIdRef();
  bool unitRef = d.isSetUnitRef();
  bool metaidRef = d.isSetMetaIdRef();
  bool portRef = d.isSetPortRef();

  bool fail = false;

  msg = "<Deletion> '";
  msg += d.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                    (d.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                     (d.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to ";

  if (idRef == true)
  {
    msg += "object with id '";
    msg += d.getIdRef();
    msg += "'";
    if (unitRef == true)
    {
      fail = true;
      msg += "and also unit with id '";
      msg += d.getUnitRef();
      msg += "'";

      if ( metaidRef == true)
      {
        msg += "and also object with metaid '";
        msg += d.getMetaIdRef();
        msg += "'";
      }

      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += d.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += d.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += d.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += d.getMetaIdRef();
      msg += "'.";
    }
  }
  else if (unitRef == true)
  {
    msg += "unit with id '";
    msg += d.getUnitRef();
    msg += "' and also ";
    
    if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += d.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += d.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += d.getMetaIdRef();
      msg += "'.";
    }
  }
  else if (metaidRef == true)
  {
    msg += "object with metaid '";
    msg += d.getMetaIdRef();
    msg += "'";

    if (portRef == true)
    {
      fail = true;
      msg += "and also port with id '";
      msg += d.getPortRef();
      msg += "'";
    }
    msg += ".";
  }

  inv(fail == false);

}
END_CONSTRAINT


//20903 - caught at read

//*************************************

//ReplacedElement constraints

//21001
START_CONSTRAINT (CompReplacedElementMustRefObject, ReplacedElement, repE)
{
  pre (repE.isSetSubmodelRef());

  bool idRef = repE.isSetIdRef();
  bool unitRef = repE.isSetUnitRef();
  bool metaidRef = repE.isSetMetaIdRef();
  bool portRef  = repE.isSetPortRef();
  bool deletion = repE.isSetDeletion();

  msg = "A <replacedElement> in ";
  const Model* mod = static_cast<const Model*>
                                  (repE.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                   (repE.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg = " does not refer to another object.";

  bool fail = true;

  if (idRef == true)
  {
    fail = false;
  }
  else if (unitRef == true)
  {
    fail = false;
  }
  else if (metaidRef == true)
  {
    fail = false;
  }
  else if (portRef == true)
  {
    fail = false;
  }
  else if (deletion == true)
  {
    fail = false;
  }

  inv(fail == false);


}
END_CONSTRAINT


//21002
START_CONSTRAINT (CompReplacedElementMustRefOnlyOne, ReplacedElement, repE)
{
  pre (repE.isSetSubmodelRef());

  bool idRef = repE.isSetIdRef();
  bool unitRef = repE.isSetUnitRef();
  bool metaidRef = repE.isSetMetaIdRef();
  bool portRef  = repE.isSetPortRef();
  bool deletion = repE.isSetDeletion();

  bool fail = false;

  msg = "<replacedElement> '";
  msg += repE.getId() ;
  msg += "' in ";
  const Model* mod = static_cast<const Model*>
                                (repE.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                  (repE.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to ";

  if (idRef == true)
  {
    msg += "object with id '";
    msg += repE.getIdRef();
    msg += "'";
    if (unitRef == true)
    {
      fail = true;
      msg += "and also unit with id '";
      msg += repE.getUnitRef();
      msg += "'";

      if ( metaidRef == true)
      {
        msg += "and also object with metaid '";
        msg += repE.getMetaIdRef();
        msg += "'";
      }

      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repE.getPortRef();
        msg += "'";
      }

      if (deletion == true)
      {
        msg += "and also deletion object '";
        msg += repE.getDeletion();
        msg += "'";
      }
      msg += ".";
    }
    else if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repE.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repE.getPortRef();
        msg += "'";
      }

      if (deletion == true)
      {
        msg += "and also deletion object '";
        msg += repE.getDeletion();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repE.getMetaIdRef();

      if (deletion == true)
      {
        msg += "and also deletion object '";
        msg += repE.getDeletion();
        msg += "'";
      }
      msg += "'.";
    }
    else if (deletion == true)
    {
      fail = true;
      msg += "and also deletion object '";
      msg += repE.getDeletion();
      msg += "'.";
    }
  }
  else if (unitRef == true)
  {
    msg += "unit with id '";
    msg += repE.getUnitRef();
    msg += "' and also ";
    
    if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repE.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repE.getPortRef();
        msg += "'";
      }
 
      if (deletion == true)
      {
        msg += "and also deletion object '";
        msg += repE.getDeletion();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repE.getMetaIdRef();
 
      if (deletion == true)
      {
        msg += "and also deletion object '";
        msg += repE.getDeletion();
        msg += "'";
      }
      msg += "'.";
    }
    else if (deletion == true)
    {
      fail = true;
      msg += "and also deletion object '";
      msg += repE.getDeletion();
      msg += "'.";
   }
  }
  else if (metaidRef == true)
  {
    msg += "object with metaid '";
    msg += repE.getMetaIdRef();
    msg += "'";

    if (portRef == true)
    {
      fail = true;
      msg += "and also port with id '";
      msg += repE.getPortRef();
      msg += "'";
    }
 
    if (deletion == true)
    {
      msg += "and also deletion object '";
      msg += repE.getDeletion();
      msg += "'";
    }
    msg += ".";
  }
  else if (portRef == true)
  {
    msg += "port with id '";
    msg += repE.getPortRef();
    msg += "'";

    if (deletion == true)
    {
      fail = true;
      msg += "and also deletion object '";
      msg += repE.getDeletion();
      msg += "'";
    }
     msg += ".";
  }

  inv(fail == false);

}
END_CONSTRAINT

//21003 - caught at read

//21004
START_CONSTRAINT (CompReplacedElementSubModelRef, ReplacedElement, repE)
{
  pre (repE.isSetSubmodelRef());

  msg = "The <replacedElement> refers to the submodel '";
  msg += repE.getSubmodelRef();
  msg += "' that is not part of the parent model.";

  bool fail = false;

  const CompModelPlugin * plug = 
                  static_cast<const CompModelPlugin*>(m.getPlugin("comp"));
  if (plug != NULL
    && plug->getSubmodel(repE.getSubmodelRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT


//21005
START_CONSTRAINT (CompReplacedElementDeletionRef, ReplacedElement, repE)
{
  pre (repE.isSetSubmodelRef());
  pre (repE.isSetDeletion());

  msg = "A <replacedElement> in ";
  const Model* mod = static_cast<const Model*>
                                (repE.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                 (repE.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg = " refers to the deletion '";
  msg += repE.getDeletion();
  msg += "' that is not part of the parent model.";

  bool fail = false;

  const CompModelPlugin * plug = 
    static_cast<const CompModelPlugin*>(m.getPlugin("comp"));
  if (plug != NULL)
  {
    const Submodel * sub  = plug->getSubmodel(repE.getSubmodelRef());

    if (sub != NULL && sub->getDeletion(repE.getDeletion()) == NULL)
    {
      fail = true;
    }
  }

  inv(fail == false);
}
END_CONSTRAINT

//21006
START_CONSTRAINT (CompReplacedElementConvFactorRef, ReplacedElement, repE)
{
  pre (repE.isSetSubmodelRef());
  pre (repE.isSetConversionFactor());

  msg = "The 'timeConversionFactor' of a <replacedElement> in ";
  const Model* mod = static_cast<const Model*>
                                 (repE.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                  (repE.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg = " is set to '";
  msg += repE.getConversionFactor();
  msg += "' which is not a <parameter> within the model.";

  bool fail = false;

  if (m.getParameter(repE.getConversionFactor()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT

//21007 - repeat of 10308
//21008 - repeat of 10309
//21009 - repeat of 10310

//TODO: 21010 - to do

//*************************************

//ReplacedBy constraints

//21101
START_CONSTRAINT (CompReplacedByMustRefObject, ReplacedBy, repBy)
{
  pre (repBy.isSetSubmodelRef());

  bool idRef = repBy.isSetIdRef();
  bool unitRef = repBy.isSetUnitRef();
  bool metaidRef = repBy.isSetMetaIdRef();
  bool portRef = repBy.isSetPortRef();

  msg = "A <replacedBy> in ";
  const Model* mod = static_cast<const Model*>
                                (repBy.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                 (repBy.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " does not refer to another object.";

  bool fail = true;

  if (idRef == true)
  {
    fail = false;
  }
  else if (unitRef == true)
  {
    fail = false;
  }
  else if (metaidRef == true)
  {
    fail = false;
  }
  else if (portRef == true)
  {
    fail = false;
  }

  inv(fail == false);
}
END_CONSTRAINT


//21102
START_CONSTRAINT (CompReplacedByMustRefOnlyOne, ReplacedBy, repBy)
{
  pre (repBy.isSetSubmodelRef());

  bool idRef = repBy.isSetIdRef();
  bool unitRef = repBy.isSetUnitRef();
  bool metaidRef = repBy.isSetMetaIdRef();
  bool portRef = repBy.isSetPortRef();

  bool fail = false;

  msg = "A <replacedBy> object in ";
  const Model* mod = static_cast<const Model*>
                                (repBy.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                 (repBy.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to ";

  if (idRef == true)
  {
    msg += "object with id '";
    msg += repBy.getIdRef();
    msg += "'";
    if (unitRef == true)
    {
      fail = true;
      msg += "and also unit with id '";
      msg += repBy.getUnitRef();
      msg += "'";

      if ( metaidRef == true)
      {
        msg += "and also object with metaid '";
        msg += repBy.getMetaIdRef();
        msg += "'";
      }

      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repBy.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repBy.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repBy.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repBy.getMetaIdRef();
      msg += "'.";
    }
  }
  else if (unitRef == true)
  {
    msg += "unit with id '";
    msg += repBy.getUnitRef();
    msg += "' and also ";
    
    if (metaidRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repBy.getMetaIdRef();
      msg += "'";
 
      if (portRef == true)
      {
        msg += "and also port with id '";
        msg += repBy.getPortRef();
        msg += "'";
      }
      msg += ".";
    }
    else if (portRef == true)
    {
      fail = true;
      msg += "and also object with metaid '";
      msg += repBy.getMetaIdRef();
      msg += "'.";
    }
  }
  else if (metaidRef == true)
  {
    msg += "object with metaid '";
    msg += repBy.getMetaIdRef();
    msg += "'";

    if (portRef == true)
    {
      fail = true;
      msg += "and also port with id '";
      msg += repBy.getPortRef();
      msg += "'";
    }
    msg += ".";
  }

  inv(fail == false);

}
END_CONSTRAINT

//21103 - caught at read

//21104
START_CONSTRAINT (CompReplacedBySubModelRef, ReplacedBy, repBy)
{
  pre (repBy.isSetSubmodelRef());

  msg = "A <replacedBy> in ";
  const Model* mod = static_cast<const Model*>
                                (repBy.getAncestorOfType(SBML_MODEL, "core"));
  if (mod == NULL) {
    mod = static_cast<const Model*>
                   (repBy.getAncestorOfType(SBML_COMP_MODELDEFINITION, "comp"));
  }
  if (mod == NULL || !mod->isSetId()) {
    msg += "the main model in the document";
  }
  else {
    msg += "the model '";
    msg += mod->getId();
    msg += "'";
  }
  msg += " refers to the submodel '";
  msg += repBy.getSubmodelRef();
  msg += "' that is not part of the parent model.";

  bool fail = false;

  const CompModelPlugin * plug = 
                      static_cast<const CompModelPlugin*>(m.getPlugin("comp"));
  if (plug != NULL
    && plug->getSubmodel(repBy.getSubmodelRef()) == NULL)
  {
    fail = true;
  }

  inv(fail == false);
}
END_CONSTRAINT




/** @endcond */
