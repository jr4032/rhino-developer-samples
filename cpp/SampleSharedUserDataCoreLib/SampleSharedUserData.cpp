#include "StdAfx.h"
#include "SampleSharedUserData.h"

// Opennurbs classes that are saved in .3dm files require
// an ON_OBJECT_IMPLEMENT() call in their implmentation.
// Use Microsoft's guidgen to create the id.
ON_OBJECT_IMPLEMENT(CSampleSharedUserData, ON_UserData, "E97E9314-899A-4045-BF9D-80DC1C9BC892");

ON_UUID CSampleSharedUserData::Id()
{
  return ON_CLASS_ID(CSampleSharedUserData);
}

ON_UUID CSampleSharedUserData::PlugInId()
{
  // CRITICAL: Copied from SampleSharedUserData1PlugIn.cpp
  // {A6BEE252-EA89-4004-82DD-67A022911E9B}
  return ON_UuidFromString("A6BEE252-EA89-4004-82DD-67A022911E9B");
}

CSampleSharedUserData::CSampleSharedUserData()
{
  m_userdata_uuid = CSampleSharedUserData::Id();

  /*
  CRITICAL:
    m_application_uuid must be assigned the uuid of the plug-in
    that will be responsible for reading and writing our user data.
    In this example, we'll use PlugIn1 as our primary plug-in.
  */
  m_application_uuid = CSampleSharedUserData::PlugInId();

  m_userdata_copycount = 1; // enable copying

  // initialize your data members here
  m_point.Set(0.0, 0.0, 0.0);
  m_string = L"";
}

CSampleSharedUserData::~CSampleSharedUserData()
{
}

CSampleSharedUserData::CSampleSharedUserData(const CSampleSharedUserData& src) : ON_UserData(src)
{
  m_userdata_uuid = CSampleSharedUserData::Id();

  /*
  CRITICAL:
    m_application_uuid must be assigned the uuid of the plug-in
    that will be responsible for reading and writing our user data.
    In this example, we'll use PlugIn1 as our primary plug-in.
  */
  m_application_uuid = CSampleSharedUserData::PlugInId();

  // All other ON_UserData fields are correctly initialized by 
  // the ON_UserData(src) call. In fact, if src is 
  // a CSampleSharedUserData class, then ALL ON_UserData fields are 
  // correctly initialized by ON_UserData(src).  
  // The next lines are here to handle the rare case 
  // when a class derived from CSampleSharedUserData is passed as src.

  // copy your data members here.
  m_point = src.m_point;
  m_string = src.m_string;
}

CSampleSharedUserData& CSampleSharedUserData::operator=(const CSampleSharedUserData& src)
{
  // NEVER set m_userdata_uuid or m_application_uuid
  // in an operator=.  These values are set when the
  // class is constructed and should not be changed.
  if (this != &src)
  {
    // Use the base class to copy all appropriate 
    // ON_UserData fields.
    ON_UserData::operator=(src);

    // Explictly copy ONLY members you added to
    // the CSampleSharedUserData class.
    m_point = src.m_point;
    m_string = src.m_string;
  }
  return *this;
}

bool CSampleSharedUserData::GetDescription(ON_wString& description)
{
  description = L"CSampleSharedUserData";
  return true;
}

bool CSampleSharedUserData::Archive() const
{
  // If false is returned, nothing will be saved in
  // 3dm archives.
  return true;
}

bool CSampleSharedUserData::Write(ON_BinaryArchive& binary_archive) const
{
  int minor_version = 0;
  bool rc = binary_archive.BeginWrite3dmChunk(TCODE_ANONYMOUS_CHUNK, 1, minor_version);
  if (!rc)
    return false;

  // Write class members like this
  for (;;)
  {
    // version 1.0 fields
    rc = binary_archive.WritePoint(m_point);
    if (!rc) break;

    rc = binary_archive.WriteString(m_string);
    if (!rc) break;

    // If you add data members to CSampleSharedUserData after you 
    // have released a product, simply increment minor_version
    // by one, and write the new information.  This way you
    // can enhance your user data over several versions of
    // your product without breaking file IO.  It is CRITICAL
    // that once you write something to a customer's .3dm
    // file, you continue to write it the same way for all
    // future versions.

    // version 1.1 fields added DD MMM YYYY
    // rc = binary_archive.WriteSomethingNew( ... );
    // if (!rc) break;
    // rc = binary_archive.WriteSomethingElseNew( ... );
    // if (!rc) break;

    // version 1.2 fields added DD MMM YYYY
    // rc = binary_archive.WriteAnotherSomethingNew( ... );
    // if (!rc) break;
    // rc = binary_archive.WriteAnotherSomethingElseNew( ... );
    // if (!rc) break;

    break;
  }

  // If BeginWrite3dmChunk() returns true,
  // then EndWrite3dmChunk() must be called, 
  // even if a write operation failed.
  if (!binary_archive.EndWrite3dmChunk())
    rc = false;

  return rc;
}

bool CSampleSharedUserData::Read(ON_BinaryArchive& binary_archive)
{
  int major_version = 0;
  int minor_version = 0;
  bool rc = binary_archive.BeginRead3dmChunk(TCODE_ANONYMOUS_CHUNK, &major_version, &minor_version);
  if (!rc)
    return false;

  // Read class members like this
  for (;;)
  {
    rc = (1 == major_version);
    if (!rc) break;

    // version 1.0 fields
    rc = binary_archive.ReadPoint(m_point);
    if (!rc) break;

    rc = binary_archive.ReadString(m_string);
    if (!rc) break;


    // The code in the comment below demonstrates how to
    // correctly read information added in later releases
    // of your product.
    //if ( minor_version >= 1 )
    //{
    //  // version 1.1 fields added DD MMM YYYY
    //  rc = binary_archive.ReadSomethingNew( ... );
    //  if (!rc) break;
    //  rc = binary_archive.ReadSomethingElseNew( ... );
    //  if (!rc) break;
    //  
    //  if ( minor_version >= 2 )
    //  {
    //    // version 1.2 fields added DD MMM YYYY
    //    rc = binary_archive.ReadAnotherSomethingNew( ... );
    //    if (!rc) break;
    //    rc = binary_archive.ReadAnotherSomethingElseNew( ... );
    //    if (!rc) break;
    //  }
    //}

    break;
  }

  // If BeginRead3dmChunk() returns true,
  // then EndRead3dmChunk() must be called, 
  // even if a read operation failed.
  if (!binary_archive.EndRead3dmChunk())
    rc = false;

  return rc;
}

/////////////////////////////////////////////////////////////////////////////
// Exported functions

bool HasPlugInUserData(const CRhinoObject* object)
{
  // Validate the object
  if (0 == object)
    return false;

  // Validate the geometry
  const ON_Geometry* geometry = object->Geometry();
  if (0 == geometry)
    return false;

  // Get our user data
  CSampleSharedUserData* user_data = CSampleSharedUserData::Cast(geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
    return false;

  return true;
}

bool AddPlugInUserData(CRhinoDoc& doc, const CRhinoObject* object, ON_3dPoint point, ON_wString string)
{
  // Validate the object
  if (0 == object)
    return false;

  // Validate the geometry
  const ON_Geometry* geometry = object->Geometry();
  if (0 == geometry)
    return false;

  // Get our user data
  CSampleSharedUserData* user_data = CSampleSharedUserData::Cast(geometry->GetUserData(CSampleSharedUserData::Id()));
  if (user_data)
    return SetPlugInUserData(doc, object, point, string);

  // Duplicate the object
  CRhinoObject* dupe_object = object->DuplicateRhinoObject();
  if (0 == dupe_object)
    return false;

  // Get the duplicated geometry
  ON_Geometry* dupe_geometry = const_cast<ON_Geometry*>(dupe_object->Geometry());
  if (0 == dupe_geometry)
  {
    delete dupe_object;
    return false;
  }

  // New user data
  user_data = new CSampleSharedUserData();
  if (0 == user_data)
  {
    delete dupe_object;
    return false;
  }

  // Set member variables
  user_data->m_point = point;
  user_data->m_string = string;

  // Attach user data
  if (0 == dupe_geometry->AttachUserData(user_data))
  {
    delete user_data;
    delete dupe_object;
    return false;
  }

  // Replace the object
  if (!doc.ReplaceObject(CRhinoObjRef(object), dupe_object))
  {
    // user_data will be deleted by dupe_object
    delete dupe_object;
    return false;
  }

  return true;
}

bool SetPlugInUserData(CRhinoDoc& doc, const CRhinoObject* object, ON_3dPoint point, ON_wString string)
{
  // Validate the object
  if (0 == object)
    return false;

  // Validate the geometry
  const ON_Geometry* geometry = object->Geometry();
  if (0 == geometry)
    return false;

  // Get our user data
  CSampleSharedUserData* user_data = CSampleSharedUserData::Cast(geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
    return false;

  // Reset pointer
  user_data = 0;

  // Duplicate the object
  CRhinoObject* dupe_object = object->DuplicateRhinoObject();
  if (0 == dupe_object)
    return false;

  // Get the duplicated geometry
  ON_Geometry* dupe_geometry = const_cast<ON_Geometry*>(dupe_object->Geometry());
  if (0 == dupe_geometry)
  {
    delete dupe_object;
    return false;
  }

  // Get the duplicated user data
  user_data = CSampleSharedUserData::Cast(dupe_geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
  {
    delete dupe_object;
    return false;
  }

  // Set member variables
  user_data->m_point = point;
  user_data->m_string = string;

  // Replace the object
  if (!doc.ReplaceObject(CRhinoObjRef(object), dupe_object))
  {
    delete dupe_object;
    return false;
  }

  return true;
}

bool GetPlugInUserData(const CRhinoObject* object, ON_3dPoint& point, ON_wString& string)
{
  // Validate the document and the object
  if (0 == object)
    return false;

  // Validate the geometry
  const ON_Geometry* geometry = object->Geometry();
  if (0 == geometry)
    return false;

  // Query the geometry
  CSampleSharedUserData* user_data = CSampleSharedUserData::Cast(geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
    return false;

  point = user_data->m_point;
  string = user_data->m_string;

  return true;
}

bool RemovePlugInUserData(CRhinoDoc& doc, const CRhinoObject* object)
{
  // Validate the document and the object
  if (0 == object)
    return false;

  // Validate the geometry
  const ON_Geometry* geometry = object->Geometry();
  if (0 == geometry)
    return false;

  // Query the geometry
  CSampleSharedUserData* user_data = CSampleSharedUserData::Cast(geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
    return false;

  // Reset pointer
  user_data = 0;

  // Duplicate the object
  CRhinoObject* dupe_object = object->DuplicateRhinoObject();
  if (0 == dupe_object)
    return false;

  // Get the duplicated geometry
  ON_Geometry* dupe_geometry = const_cast<ON_Geometry*>(dupe_object->Geometry());
  if (0 == dupe_geometry)
  {
    delete dupe_object;
    return false;
  }

  // Get the duplicated user data
  user_data = CSampleSharedUserData::Cast(dupe_geometry->GetUserData(CSampleSharedUserData::Id()));
  if (0 == user_data)
  {
    delete dupe_object;
    return false;
  }

  // Detach and delete the user data
  dupe_geometry->DetachUserData(user_data);
  delete user_data;
  user_data = 0;

  // Replace the object
  if (!doc.ReplaceObject(CRhinoObjRef(object), dupe_object))
  {
    delete dupe_object;
    return false;
  }

  return true;
}
