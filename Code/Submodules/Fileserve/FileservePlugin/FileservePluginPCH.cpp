#include <FileservePlugin/FileservePluginPCH.h>

NS_STATICLINK_LIBRARY(FileservePlugin)
{
  if (bReturn)
    return;

  NS_STATICLINK_REFERENCE(FileservePlugin_Client_FileserveClient);
  NS_STATICLINK_REFERENCE(FileservePlugin_Client_FileserveDataDir);
  NS_STATICLINK_REFERENCE(FileservePlugin_Fileserver_ClientContext);
  NS_STATICLINK_REFERENCE(FileservePlugin_Fileserver_Fileserver);
  NS_STATICLINK_REFERENCE(FileservePlugin_Main);
}
