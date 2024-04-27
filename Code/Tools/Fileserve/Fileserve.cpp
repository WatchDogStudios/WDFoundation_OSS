#include <Fileserve/FileservePCH.h>

#include <Fileserve/Fileserve.h>
#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/Utilities/CommandLineUtils.h>

#ifdef NS_USE_QT
#  include <Fileserve/Gui.moc.h>
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#  include <QApplication>
#endif

#ifdef NS_USE_QT
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int iCmdShow)
{
#else
int main(int argc, const char** argv)
{
#endif
  nsFileserverApp* pApp = new nsFileserverApp();

#ifdef NS_USE_QT
  nsCommandLineUtils::GetGlobalInstance()->SetCommandLine();

  int argc = 0;
  char** argv = nullptr;
  QApplication* pQtApplication = new QApplication(argc, const_cast<char**>(argv));
  pQtApplication->setApplicationName("WDFileserver");
  pQtApplication->setOrganizationDomain("www.wdstudios.tech");
  pQtApplication->setOrganizationName("WD Studios L.L.C.");
  pQtApplication->setApplicationVersion("1.0.0");

  nsRun_Startup(pApp).IgnoreResult();

  CreateFileserveMainWindow(pApp);
  pQtApplication->exec();
  nsRun_Shutdown(pApp);
#else
  pApp->SetCommandLineArguments((nsUInt32)argc, argv);
  nsRun(pApp);
#endif

  const int iReturnCode = pApp->GetReturnCode();
  if (iReturnCode != 0)
  {

    std::string text = pApp->TranslateReturnCode();
    if (!text.empty())
      printf("Return Code: '%s'\n", text.c_str());
  }

#ifdef NS_USE_QT
  delete pQtApplication;
#endif

  delete pApp;


  return iReturnCode;
}

nsResult nsFileserverApp::BeforeCoreSystemsStartup()
{
  nsStartup::AddApplicationTag("tool");
  nsStartup::AddApplicationTag("fileserve");

  return SUPER::BeforeCoreSystemsStartup();
}

void nsFileserverApp::FileserverEventHandler(const nsFileserverEvent& e)
{
  switch (e.m_Type)
  {
    case nsFileserverEvent::Type::ClientConnected:
    case nsFileserverEvent::Type::ClientReconnected:
      ++m_uiConnections;
      m_TimeTillClosing = nsTime::MakeZero();
      break;
    case nsFileserverEvent::Type::ClientDisconnected:
      --m_uiConnections;

      if (m_uiConnections == 0 && m_CloseAppTimeout.GetSeconds() > 0)
      {
        // reset the timer
        m_TimeTillClosing = nsTime::Now() + m_CloseAppTimeout;
      }

      break;
    default:
      break;
  }
}
