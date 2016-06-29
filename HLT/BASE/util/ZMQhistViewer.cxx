#include <TApplication.h>
#include <TGClient.h>
#include "AliZMQMTviewerGUI.h"
#include <cstdlib>

int main(int argc, char **argv) {
   char* fDISPLAY = std::getenv("DISPLAY");
   if (!fDISPLAY) {
     printf("$DISPLAY not set, exiting...\n");
     return 1;
   }
   TApplication theApp("App", NULL, NULL);
   AliZMQMTviewerGUI viewer(gClient->GetRoot(), 200, 200, argc, argv);
   if (viewer.GetInitStatus()<0) {
     printf("%s",viewer.fUSAGE);
     return 1;
   }
   theApp.Run();
   return 0;
}

