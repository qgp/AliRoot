//
//  AliEveSaveViews.cxx
//
//
//  Created by Jeremi Niedziela on 16/03/15.
//
//

#include "AliEveSaveViews.h"
#include "AliEveMultiView.h"
#include "AliEveInit.h"

#include <TGFileDialog.h>
#include <TGLViewer.h>
#include <TEveViewer.h>
#include <TEveManager.h>
#include <TEveBrowser.h>
#include <TMath.h>
#include <TTimeStamp.h>
#include <TEnv.h>
#include <TSQLServer.h>
#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSystem.h>

#include <AliRawReader.h>
#include <AliRawEventHeaderBase.h>
#include <AliEveEventManager.h>
#include <AliDAQ.h>

#include <iostream>
using namespace std;


AliEveSaveViews::AliEveSaveViews(int width,int height) :
fHeightInfoBar(0.0722*height),
fWidth(width),
fHeight(height)
{
    fRunNumber=-1;
    fCurrentFileNumber=0;
    fNumberOfClusters=-1;
    fMaxFiles=100;
    fCompositeImgFileName="";
    
    for (int i=0; i<3; i++) {
        fTriggerClasses[i]="";
    }
    fClustersInfo="";
}


AliEveSaveViews::~AliEveSaveViews()
{
    
}

void AliEveSaveViews::ChangeRun()
{
    // read crededentials to logbook from config file:
    TEnv settings;
    AliEveInit::GetConfig(&settings);
    const char *dbHost = settings.GetValue("logbook.host", "");
    Int_t   dbPort =  settings.GetValue("logbook.port", 0);
    const char *dbName =  settings.GetValue("logbook.db", "");
    const char *user =  settings.GetValue("logbook.user", "");
    const char *password = settings.GetValue("logbook.pass", "");
    
    // get entry from logbook:
    cout<<"creating sql server...";
    TSQLServer* server = TSQLServer::Connect(Form("mysql://%s:%d/%s", dbHost, dbPort, dbName), user, password);
    if(!server){
        cout<<"Server could not be created"<<endl;
        return;
    }
    else{
        cout<<"created"<<endl;
    }
    TString sqlQuery;
    sqlQuery.Form("SELECT * FROM logbook_trigger_clusters WHERE run = %d",fRunNumber);
    
    TSQLResult* result = server->Query(sqlQuery);
    if (!result){
        Printf("ERROR: Can't execute query <%s>!", sqlQuery.Data());
        return;
    }
    
    if (result->GetRowCount() == 0){
        Printf("ERROR: Run %d not found", fRunNumber);
        delete result;
        return;
    }
    
    // read values from logbook entry:
    TSQLRow* row;
    int i=0;
    fCluster.clear();
    
    while((row=result->Next()))
    {
        fCluster.push_back(atoi(row->GetField(2)));
        fInputDetectorMask.push_back(atol(row->GetField(4)));
        i++;
    }
    fNumberOfClusters=i;
}

void AliEveSaveViews::SaveForAmore()
{
    cout<<"\n\nSaving views for AMORE\n"<<endl;
    
    gEve->GetBrowser()->RaiseWindow();
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    
    if(AliEveEventManager::HasESD())
    {
        fESDEvent = AliEveEventManager::Instance()->AssertESD();
    }
    else
    {
        cout<<"AliEveSaveViews -- event manager has no esd file"<<endl;
        return;
    }
    
    if(fESDEvent->GetRunNumber()!=fRunNumber)
    {
        fRunNumber = fESDEvent->GetRunNumber();
        ChangeRun();
    }
    
    TEveViewerList* viewers = gEve->GetViewers();
    int Nviewers = viewers->NumChildren()-2; // remark: 3D view is counted twice
    
    TASImage *compositeImg = new TASImage(fWidth, fHeight+1.33*fHeightInfoBar);
    
    // 3D View size
    int width3DView = TMath::FloorNint(2.*fWidth/3.); // the width of the 3D view
    int height3DView= fHeight-fHeightInfoBar;                                       // the height of the 3D view
    float aspectRatio = (float)width3DView/(float)height3DView;                     // 3D View aspect ratio
    
    // Children View Size
    int heightChildView = TMath::FloorNint((float)fHeight/Nviewers);
    int widthChildView  = TMath::FloorNint((float)fWidth/3.);
    float childAspectRatio = (float)widthChildView/(float)heightChildView; // 3D View aspect ratio
    
    int index=0;            // iteration counter
    int x = width3DView;    // x position of the child view
    int y = 0;              // y position of the child view
    TString viewFilename;   // save view to this file
    
     for(TEveElement::List_i i = viewers->BeginChildren(); i != viewers->EndChildren(); i++)
    { // NB: this skips the first children (first 3D View)
        TEveViewer* view = ((TEveViewer*)*i);
        
        if((strcmp(view->GetName(),"3D View MV")!=0) &&
           (strcmp(view->GetName(),"RPhi View")!=0) &&
           (strcmp(view->GetName(),"RhoZ View")!=0)){
            continue;
        }
        
        viewFilename = Form("view-%d.png", index);
        
        // Save OpenGL view in file and read it back using BB (missing method in Root returning TASImage)
//        view->GetGLViewer()->SavePictureUsingBB(viewFilename);
//        TASImage *viewImg = new TASImage(viewFilename);
        
        //        tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingBB();
        
        
        // Second option is to use FBO instead of BB
        // This improves the quality of pictures in some specific cases
        // but is causes a bug (moving mouse over views makes them disappear
        // on new event being loaded
        
        TASImage *viewImg;
        if(index==0){
            viewImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(width3DView, height3DView);
        }
        else {
            viewImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(widthChildView, heightChildView);
        }
        
        if(viewImg){
            // copy view image in the composite image
            int currentWidth = viewImg->GetWidth();
            int currentHeight = viewImg->GetHeight();
            
            if(index==0){
                if(currentWidth < aspectRatio*currentHeight)
                {
                    viewImg->Crop(0,(currentHeight-currentWidth/aspectRatio)*0.5,currentWidth,currentWidth/aspectRatio);
                }
                else
                {
                    viewImg->Crop((currentWidth-currentHeight*aspectRatio)*0.5,0,currentHeight*aspectRatio,currentHeight);
                }
                
                viewImg->Scale(width3DView,height3DView);
                viewImg->CopyArea(compositeImg, 0,0, width3DView, height3DView);
                
            }
            else
            {
                if(currentWidth < aspectRatio*currentHeight)
                {
                    viewImg->Crop(0,(currentHeight-currentWidth/childAspectRatio)*0.5,currentWidth,currentWidth/childAspectRatio);
                }
                else
                {
                    viewImg->Crop((currentWidth-currentHeight*childAspectRatio)*0.5,0,currentHeight*childAspectRatio,currentHeight);
                }
                viewImg->Scale(widthChildView,heightChildView);
                viewImg->CopyArea(compositeImg,0,0, widthChildView, heightChildView, x,y);
                compositeImg->DrawRectangle(x,y, widthChildView, heightChildView, "#C0C0C0"); // draw a border around child views
            }
            delete viewImg;viewImg=0;
        }
        if(index>0){ // skip 3D View
            y+=heightChildView;
        }
        index++;
    }
    
    //---------------------------------------------------
    //              show LIVE bar
    //---------------------------------------------------
    
    TTimeStamp ts;
    UInt_t year,month,day;
    UInt_t hour,minute,second;
    
    ts.GetDate(kFALSE, 0, &year, &month,&day);
    ts.GetTime(kFALSE, 0, &hour, &minute,&second);
    
    TString gmtNow = TString::Format("%u-%.2u-%.2u %.2u:%.2u:%.2u",year,month,day,hour,minute,second);
    
    compositeImg->Gradient( 90, "#EAEAEA #D2D2D2 #FFFFFF", 0, 0.03*fWidth, 0.000*fHeight, 0.20*fWidth, 0.10*fHeight);
    compositeImg->Gradient( 90, "#D6D6D6 #242424 #000000", 0, 0.08*fWidth, 0.066*fHeight, 0.15*fWidth, 0.03*fHeight);
    compositeImg->BeginPaint();
    compositeImg->DrawRectangle(0.01*fWidth,0*fHeight, 0.23*fWidth, 0.1*fHeight);
    compositeImg->DrawText(0.11*fWidth, 0.005*fHeight, "LIVE", 0.08*fHeight, "#FF2D00", "FreeSansBold.otf");
    compositeImg->DrawText(0.09*fWidth, 0.073*fHeight, gmtNow, 0.02*fHeight, "#FFFFFF", "arial.ttf");
    compositeImg->DrawText(0.20*fWidth, 0.073*fHeight, "Geneva", 0.01*fHeight, "#FFFFFF", "arial.ttf");
    compositeImg->DrawText(0.20*fWidth, 0.083*fHeight, "time", 0.01*fHeight, "#FFFFFF", "arial.ttf");
    compositeImg->EndPaint();
    //include ALICE Logo
    TASImage *aliceLogo = new TASImage(Form("%s/EVE/resources/alice_logo.png",gSystem->Getenv("ALICE_ROOT")));
    if(aliceLogo)
    {
        aliceLogo->Scale(0.037*fWidth,319./236.*0.037*fWidth);
        compositeImg->Merge(aliceLogo, "alphablend", 0.034*fWidth, 0.004*fHeight);
        delete aliceLogo;aliceLogo=0;
    }
    
    
    //---------------------------------------------------
    //              show Information bar
    //---------------------------------------------------
    
    BuildEventInfoString();
    BuildTriggerClassesStrings();
    BuildClustersInfoString();
    
    // put event's info in blue bar on the bottom:
    compositeImg->Gradient( 90, "#1B58BF #1D5CDF #0194FF", 0, 0,fHeight, fWidth, fHeightInfoBar);
    compositeImg->BeginPaint();
    compositeImg->DrawText(0.007*fWidth, 1.0166*fHeight, fEventInfo, 0.03*fHeight, "#FFFFFF", "FreeSansBold.otf");
    // put trigger classes in blue bar on the bottom:
    compositeImg->DrawText(0.52 *fWidth, 1.0044*fHeight, fTriggerClasses[0], 0.0177*fHeight, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(0.52 *fWidth, 1.0244*fHeight, fTriggerClasses[1], 0.0177*fHeight, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(0.52 *fWidth, 1.0488*fHeight, fTriggerClasses[2], 0.0177*fHeight, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(0.007*fWidth, 0.9333*fHeight, "Preliminary reconstruction", 0.03*fHeight, "#60FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(0.007*fWidth, 0.9666*fHeight, "(not for publication)", 0.03*fHeight, "#60FFFFFF", "FreeSansBold.otf");
    compositeImg->EndPaint();
    // put clusters description in green bar on the bottom:
    compositeImg->Gradient( 90, "#1BDD1B #1DDD1D #01DD01", 0, 0, fHeight+fHeightInfoBar, fWidth, 0.33*fHeightInfoBar);
    compositeImg->BeginPaint();
    compositeImg->DrawText(0.007*fWidth,fHeight+fHeightInfoBar+2,fClustersInfo, 0.0177*fHeight, "#000000", "FreeSansBold.otf");
    compositeImg->EndPaint();
    
    // write composite image to disk
    fCompositeImgFileName = Form("online-viz-%03d", fCurrentFileNumber);
    TASImage *imgToSave = new TASImage(fWidth,fHeight+1.33*fHeightInfoBar);
    compositeImg->CopyArea(imgToSave, 0,0, fWidth, fHeight+1.33*fHeightInfoBar);
    
    imgToSave->WriteImage(Form("%s.png", fCompositeImgFileName.Data()));
    
    cout<<"Fix the scale"<<endl;
    TEveBrowser *browser = gEve->GetBrowser();
    browser->MoveResize(browser->GetX(), browser->GetY(), browser->GetWidth(),browser->GetHeight());
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    gEve->Redraw3D(true);
    cout<<"rescaled"<<endl;
    
    if(compositeImg){delete compositeImg;compositeImg=0;}
    if(imgToSave){delete imgToSave;imgToSave=0;}
    if (++fCurrentFileNumber >= fMaxFiles) fCurrentFileNumber = 0;
}

void AliEveSaveViews::Save(bool withDialog,char* filename)
{
    TEnv settings;
    AliEveInit::GetConfig(&settings);
    
    bool logo        = settings.GetValue("screenshot.logo.draw",true);
    bool info        = settings.GetValue("screenshot.info.draw",true);
    bool projections = settings.GetValue("screenshot.projections.draw",true);
    const char *energyLabel= settings.GetValue("screenshot.force.energy","Energy: unknown");
    const char *systemLabel= settings.GetValue("screenshot.force.system","System: unknown");
    if(strcmp(energyLabel,"")==0)energyLabel="Energy: unknown";
    if(strcmp(systemLabel,"")==0)systemLabel="System: unknown";
    
    gEve->GetBrowser()->RaiseWindow();
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    
    if(AliEveEventManager::HasESD())
    {
        fESDEvent = AliEveEventManager::Instance()->AssertESD();
    }
    else
    {
        cout<<"AliEveSaveViews -- event manager has no esd file"<<endl;
        return;
    }
    
    TEveViewerList* viewers = gEve->GetViewers();
    int Nviewers = viewers->NumChildren()-2; // remark: 3D view is counted twice
    
    int width = 3840;
    int height= 2160;
    
    // 3,840 × 2,160 (4K)
    // 7,680 × 4,320 (8K)
    // 15,360 × 8,640 (16K)
    
    TASImage *compositeImg = new TASImage(width, height);
    
    // 3D View size
    int width3DView = projections ? TMath::FloorNint(2.*width/3.) : width;            // the width of the 3D view
    int height3DView= height;                                   // the height of the 3D view
    float aspectRatio = (float)width3DView/(float)height3DView; // 3D View aspect ratio
    
    // Children View Size
    int heightChildView = TMath::FloorNint((float)height/Nviewers);
    int widthChildView  = TMath::FloorNint((float)width/3.);
    float childAspectRatio = (float)widthChildView/(float)heightChildView; // 3D View aspect ratio
    
    int index=0;            // iteration counter
    int x = width3DView;    // x position of the child view
    int y = 0;              // y position of the child view
    TString viewFilename;   // save view to this file
    
    for(TEveElement::List_i i = viewers->BeginChildren(); i != viewers->EndChildren(); i++)
    { // NB: this skips the first children (first 3D View)
        TEveViewer* view = ((TEveViewer*)*i);
        
        if((strcmp(view->GetName(),"3D View MV")!=0) &&
           (strcmp(view->GetName(),"RPhi View")!=0) &&
           (strcmp(view->GetName(),"RhoZ View")!=0)){
            continue;
        }
        
        viewFilename = Form("view-%d.png", index);
        
        
        // Save OpenGL view in file and read it back using BB (missing method in Root returning TASImage)
        //        view->GetGLViewer()->SavePictureUsingBB(viewFilename);
        //        TASImage *viewImg = new TASImage(viewFilename);
        
        //        tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingBB();
        
        
        // Second option is to use FBO instead of BB
        // This improves the quality of pictures in some specific cases
        // but is causes a bug (moving mouse over views makes them disappear
        // on new event being loaded
        
        TASImage *viewImg;
        if(index==0){
            viewImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(width3DView, height3DView);
        }
        else {
            viewImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(widthChildView, heightChildView);
        }
        
        if(viewImg){
            // copy view image in the composite image
            int currentWidth = viewImg->GetWidth();
            int currentHeight = viewImg->GetHeight();
            
            if(index==0){
                if(currentWidth < aspectRatio*currentHeight)
                {
                    viewImg->Crop(0,(currentHeight-currentWidth/aspectRatio)*0.5,currentWidth,currentWidth/aspectRatio);
                }
                else
                {
                    viewImg->Crop((currentWidth-currentHeight*aspectRatio)*0.5,0,currentHeight*aspectRatio,currentHeight);
                }
                
                viewImg->Scale(width3DView,height3DView);
                viewImg->CopyArea(compositeImg, 0,0, width3DView, height3DView);
                
            }
            else
            {
                if(currentWidth < aspectRatio*currentHeight)
                {
                    viewImg->Crop(0,(currentHeight-currentWidth/childAspectRatio)*0.5,currentWidth,currentWidth/childAspectRatio);
                }
                else
                {
                    viewImg->Crop((currentWidth-currentHeight*childAspectRatio)*0.5,0,currentHeight*childAspectRatio,currentHeight);
                }
                viewImg->Scale(widthChildView,heightChildView);
                viewImg->CopyArea(compositeImg,0,0, widthChildView, heightChildView, x,y);
                compositeImg->DrawRectangle(x,y, widthChildView, heightChildView, "#C0C0C0"); // draw a border around child views
            }
            delete viewImg;viewImg=0;
        }
        if(index>0){ // skip 3D View
            y+=heightChildView;
        }
        index++;
        if(!projections){
            break;
        }
    }
    
    //draw ALICE Logo
    if(logo)
    {
        TASImage *aliceLogo = new TASImage(Form("%s/EVE/resources/alice_logo_big.png",gSystem->Getenv("ALICE_ROOT")));
        if(aliceLogo)
        {
            double ratio = 1434./1939.;
            aliceLogo->Scale(0.08*width,0.08*width/ratio);
            compositeImg->Merge(aliceLogo, "alphablend", 20, 20);
            delete aliceLogo;aliceLogo=0;
        }
    }
    
    //draw info
    if(info)
    {
        TTimeStamp ts(fESDEvent->GetTimeStamp());
        //    TTimeStamp offset(0,0,0,0,0,0,0,true,-2*3600);
        //    ts.Add(offset);
        const char *runNumber = Form("Run:%d",fESDEvent->GetRunNumber());
        const char *timeStamp = Form("Timestamp:%s(UTC)",ts.AsString("s"));
        const char *system;
        
        if(strcmp(fESDEvent->GetBeamType(),"")!=0)
        {
            if(strcmp(fESDEvent->GetBeamType(),"A-A"))
            {
                system = "Colliding system:Pb-Pb";
            }
            else
            {
                system = Form("Colliding system:%s",fESDEvent->GetBeamType());
            }
        }
        else
        {
            system = systemLabel;
        }
        
        system = "Colliding system:p-p";
        
        const char *energy;
        if(fESDEvent->GetBeamEnergy()>=0.0000001)
        {
            energy = Form("Energy:%.0f TeV",2*fESDEvent->GetBeamEnergy()/1000);
        }
        else
        {
            energy = energyLabel;
        }
        int fontSize = 0.015*height;
        compositeImg->BeginPaint();
        compositeImg->DrawText(10, height-25-4*fontSize, runNumber, fontSize, "#BBBBBB", "FreeSansBold.otf");
        compositeImg->DrawText(10, height-20-3*fontSize, timeStamp, fontSize, "#BBBBBB", "FreeSansBold.otf");
        compositeImg->DrawText(10, height-15-2*fontSize, system,    fontSize, "#BBBBBB", "FreeSansBold.otf");
        compositeImg->DrawText(10, height-10-1*fontSize, energy,    fontSize, "#BBBBBB", "FreeSansBold.otf");
        compositeImg->EndPaint();
    }
    // write composite image to disk
    TASImage *imgToSave = new TASImage(width,height);
    compositeImg->CopyArea(imgToSave, 0,0, width, height);
    
    // Save screenshot to file
    if(withDialog)
    {
        TGFileInfo fileinfo;
        const char *filetypes[] = {"All types", "*", 0, 0};
        fileinfo.fFileTypes = filetypes;
        fileinfo.fIniDir = StrDup(".");
        new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(),kFDOpen, &fileinfo);
        if(!fileinfo.fFilename)
        {
            cout<<"AliEveSaveViews::SaveWithDialog() -- couldn't get path from dialog window!!!"<<endl;
            return;
        }
        
        imgToSave->WriteImage(fileinfo.fFilename);
    }
    else
    {
        imgToSave->WriteImage(filename);
    }
    
    if(compositeImg){delete compositeImg;compositeImg=0;}
    if(imgToSave){delete imgToSave;imgToSave=0;}
}

void AliEveSaveViews::BuildEventInfoString()
{
    AliRawReader* rawReader = AliEveEventManager::AssertRawReader();
    if(rawReader)
    {
        fEventInfo.Form("Run: %d  Event#: %d (%s)",
                        rawReader->GetRunNumber(),
                        AliEveEventManager::CurrentEventId(),
                        AliRawEventHeaderBase::GetTypeName(rawReader->GetType())
                        );
        return;
    }
    else
    {
        AliESDEvent* esd =  AliEveEventManager::Instance()->AssertESD();
        if(esd)
        {
            fEventInfo.Form("Colliding: %s Run: %d  Event: %d (%s)",
                            esd->GetESDRun()->GetBeamType(),
                            esd->GetRunNumber(),
                            esd->GetEventNumberInFile(),
                            AliRawEventHeaderBase::GetTypeName(esd->GetEventType())
                            );
            return;
        }
        else
        {
            fEventInfo="";
        }
    }
}

void AliEveSaveViews::BuildTriggerClassesStrings()
{
    ULong64_t mask = 1;
    int sw=0;
    
    AliESDEvent* esd =  AliEveEventManager::Instance()->AssertESD();
    ULong64_t triggerMask = esd->GetTriggerMask();
    ULong64_t triggerMaskNext50 = esd->GetTriggerMaskNext50();
    
    fTriggerClasses[0]="";
    fTriggerClasses[1]="";
    fTriggerClasses[2]="";
    
    for(int clusterIter=0;clusterIter<fNumberOfClusters;clusterIter++)//loop over all clusters in run
    {
        // get trigger classes for given cluster
        mask=1;
        for(int i=0;i<50;i++)
        {
            if(mask & triggerMask)
            {
                fTriggerClasses[sw]+=esd->GetESDRun()->GetTriggerClass(i);
                fTriggerClasses[sw]+=Form("(%d)",fCluster[clusterIter]);
                fTriggerClasses[sw]+="   ";
                
                if(sw==0)sw=1;
                else if(sw==1)sw=2;
                else if(sw==2)sw=0;
            }
            if(mask & triggerMaskNext50)
            {
                fTriggerClasses[sw]+=esd->GetESDRun()->GetTriggerClass(i+50);
                fTriggerClasses[sw]+=Form("(%d)",fCluster[clusterIter]);
                fTriggerClasses[sw]+="   ";
                
                if(sw==0)sw=1;
                else if(sw==1)sw=2;
                else if(sw==2)sw=0;
            }
            mask = mask<<1;
        }
    }
}

void AliEveSaveViews::BuildClustersInfoString()
{
    vector<TString> clustersDescription;
    ULong64_t mask = 1;
    
    for(int clusterIter=0;clusterIter<fNumberOfClusters;clusterIter++)//loop over all clusters in run
    {
        string clustersInfo="";
        mask=1;
        for(int i=0;i<22;i++)
        {
            if(fInputDetectorMask[clusterIter] & mask)
            {
                clustersInfo+=AliDAQ::DetectorName(i);
                clustersInfo+=", ";
            }
            
            mask=mask<<1;
        }
        clustersInfo = clustersInfo.substr(0, clustersInfo.size()-2);
        
        clustersDescription.push_back(TString(clustersInfo));
    }
    
    fClustersInfo = "";
    
    for (int i=0;i<clustersDescription.size();i++) {
        fClustersInfo+="Cluster ";
        fClustersInfo+=fCluster[i];
        fClustersInfo+=":";
        fClustersInfo+=clustersDescription[i];
        fClustersInfo+="   ";
    }
}

int AliEveSaveViews::SendToAmore()
{
    return gSystem->Exec(Form("SendImageToAmore %s %s.png %d",fCompositeImgFileName.Data(),fCompositeImgFileName.Data(),fRunNumber));
}



