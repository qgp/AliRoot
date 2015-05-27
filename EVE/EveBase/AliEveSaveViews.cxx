//
//  AliEveSaveViews.cxx
//
//
//  Created by Jeremi Niedziela on 16/03/15.
//
//

#include "AliEveSaveViews.h"

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

#include <AliRawReader.h>
#include <AliRawEventHeaderBase.h>
#include <AliEveEventManager.h>
#include <AliDAQ.h>
#include <AliOnlineReconstructionUtil.h>

#include <iostream>
using namespace std;


AliEveSaveViews::AliEveSaveViews(int width,int height,int heightInfoBar,bool showLiveBar) :
fHeightInfoBar(heightInfoBar),
fWidth(width),
fHeight(height),
fShowLiveBar(showLiveBar)
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
    settings.ReadFile(AliOnlineReconstructionUtil::GetPathToServerConf(), kEnvUser);
    const char *dbHost = settings.GetValue("logbook.host", "");
    Int_t   dbPort =  settings.GetValue("logbook.port", 0);
    const char *dbName =  settings.GetValue("logbook.db", "");
    const char *user =  settings.GetValue("logbook.user", "");
    const char *password = settings.GetValue("logbook.pass", "");
    
    // get entry from logbook:
    cout<<"creating sql server...";
    TSQLServer* server = TSQLServer::Connect(Form("mysql://%s:%d/%s", dbHost, dbPort, dbName), user, password);
    cout<<"created"<<endl;
    
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
    while((row=result->Next()))
    {
        fCluster.push_back(atoi(row->GetField(2)));
        fInputDetectorMask.push_back(atol(row->GetField(4)));
        i++;
    }
    fNumberOfClusters=i;
}

void AliEveSaveViews::Save()
{
    gEve->GetBrowser()->RaiseWindow();
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    
    if(AliEveEventManager::HasESD())
    {
        fESDEvent = AliEveEventManager::AssertESD();
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
    
    TASImage *compositeImg = new TASImage(fWidth, fHeight);
    
    // 3D View size
    int width3DView = TMath::FloorNint((float)Nviewers*fWidth/(float)(Nviewers+1)); // the width of the 3D view
    int height3DView= fHeight-fHeightInfoBar;                                       // the height of the 3D view
    float aspectRatio = (float)width3DView/(float)height3DView;                     // 3D View aspect ratio
    
    // Children View Size
    int heightChildView = TMath::FloorNint((float)height3DView/(float)Nviewers);
    int widthChildView  = TMath::FloorNint(aspectRatio*heightChildView); // has the same aspect ratio as the 3D view
    
    int index=0;            // iteration counter
    int x = width3DView;    // x position of the child view
    int y = 0;              // y position of the child view
    TString viewFilename;   // save view to this file
    
    for(TEveElement::List_i i = (++viewers->BeginChildren()); i != viewers->EndChildren(); i++)
    { // NB: this skips the first children (first 3D View)
        TEveViewer* view = ((TEveViewer*)*i);
        viewFilename = Form("view-%d.png", index);
        
        // Save OpenGL view in file and read it back using BB (missing method in Root returning TASImage)
        view->GetGLViewer()->SavePictureUsingBB(viewFilename);
        TASImage *viewImg = new TASImage(viewFilename);
        
        //        tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingBB();
        
        
        // Second option is to use FBO instead of BB
        // This improves the quality of pictures in some specific cases
        // but is causes a bug (moving mouse over views makes them disappear
        // on new event being loaded
        
        //         if(index==0){
        //         tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(width3DView, height3DView);
        //         }
        //         else {
        //         tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(widthChildView, heightChildView);
        //         }
        //
        
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
            else {
                viewImg->Crop((currentWidth-widthChildView)*0.5,
                              (currentHeight-heightChildView)*0.5,
                              widthChildView,
                              heightChildView);
                viewImg->CopyArea(compositeImg, 0,0, widthChildView, heightChildView, x,y);
                compositeImg->DrawRectangle(x,y, widthChildView, heightChildView, "#C0C0C0"); // draw a border around child views
            }
            delete viewImg;viewImg=0;
        }
        if(index>0){ // skip 3D View
            y+=heightChildView;
        }
        index++;
    }
    /*
     // Create a glow (bloom) effect
     TASImage *tempImg = (TASImage*)compositeImg->Clone("tempImg");
     tempImg->Blur(10.0,10.0);
     compositeImg->Merge(tempImg, "lighten");
     if(tempImg){delete tempImg;tempImg=0;}
     */
    
    // show LIVE bar
    if(fShowLiveBar)
    {
        TTimeStamp ts;
        TString tNow = ts.AsString("s"); // display date & time
        
        compositeImg->Gradient( 90, "#EAEAEA #D2D2D2 #FFFFFF", 0, 75, 0, 239, 95);
        compositeImg->Gradient( 90, "#D6D6D6 #242424 #000000", 0, 155, 60, 152, 26);
        compositeImg->BeginPaint();
        compositeImg->DrawRectangle(50,0, 264, 94);
        compositeImg->DrawText(162, 6, "LIVE", 70, "#FF2D00", "FreeSansBold.otf");
        compositeImg->DrawText(162, 65, tNow, 16, "#FFFFFF", "arial.ttf");
        compositeImg->EndPaint();
        //include ALICE Logo
        TASImage *aliceLogo = new TASImage(Form("%s/EVE/macros/alice_logo.png",gSystem->Getenv("ALICE_ROOT")));
        if(aliceLogo)
        {
            aliceLogo->Scale(64,87);
            compositeImg->Merge(aliceLogo, "alphablend", 82, 4);
            delete aliceLogo;aliceLogo=0;
        }
    }
    
    //---------------------------------------------------
    //              show Information bar
    //---------------------------------------------------
    
    BuildEventInfoString();
    BuildTriggerClassesStrings();
    BuildClustersInfoString();
    
    // put event's info in blue bar on the bottom:
    compositeImg->Gradient( 90, "#1B58BF #1D5CDF #0194FF", 0, 0,fHeight-1.33*fHeightInfoBar, fWidth, fHeightInfoBar);
    compositeImg->BeginPaint();
    compositeImg->DrawText(10, fHeight-1.33*fHeightInfoBar+15, fEventInfo, 28, "#FFFFFF", "FreeSansBold.otf");
    // put trigger classes in blue bar on the bottom:
    compositeImg->DrawText(750, fHeight-1.33*fHeightInfoBar+4 ,fTriggerClasses[0], 16, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(750, fHeight-1.33*fHeightInfoBar+24,fTriggerClasses[1], 16, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->DrawText(750, fHeight-1.33*fHeightInfoBar+44,fTriggerClasses[2], 16, "#FFFFFF", "FreeSansBold.otf");
    compositeImg->EndPaint();
    // put clusters description in green bar on the bottom:
    compositeImg->Gradient( 90, "#1BDD1B #1DDD1D #01DD01", 0, 0, fHeight-0.33*fHeightInfoBar, fWidth, 0.33*fHeightInfoBar);
    compositeImg->BeginPaint();
    compositeImg->DrawText(10,fHeight-0.33*fHeightInfoBar+2,fClustersInfo, 16, "#000000", "FreeSansBold.otf");
    compositeImg->EndPaint();
    
    // write composite image to disk
    fCompositeImgFileName = Form("online-viz-%03d", fCurrentFileNumber);
    TASImage *imgToSave = new TASImage(fWidth,fHeight);
    compositeImg->CopyArea(imgToSave, 0,0, fWidth, fHeight);

    imgToSave->WriteImage(Form("%s.png", fCompositeImgFileName.Data()));
    
    if(compositeImg){delete compositeImg;compositeImg=0;}
    if(imgToSave){delete imgToSave;imgToSave=0;}
    if (++fCurrentFileNumber >= fMaxFiles) fCurrentFileNumber = 0;
}

void AliEveSaveViews::SaveWithDialog()
{
    gEve->GetBrowser()->RaiseWindow();
    gEve->FullRedraw3D();
    gSystem->ProcessEvents();
    
    if(AliEveEventManager::HasESD())
    {
        fESDEvent = AliEveEventManager::AssertESD();
    }
    else
    {
        cout<<"AliEveSaveViews -- event manager has no esd file"<<endl;
        return;
    }
    
    TEveViewerList* viewers = gEve->GetViewers();
    int Nviewers = viewers->NumChildren()-2; // remark: 3D view is counted twice
    
    int width = 3556;
    int height= 2000;
    
    TASImage *compositeImg = new TASImage(width, height);
    
    // 3D View size
    int width3DView = TMath::FloorNint(2.*width/3.);            // the width of the 3D view
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
    
    for(TEveElement::List_i i = (++viewers->BeginChildren()); i != viewers->EndChildren(); i++)
    { // NB: this skips the first children (first 3D View)
        TEveViewer* view = ((TEveViewer*)*i);
        viewFilename = Form("view-%d.png", index);
        
        // Save OpenGL view in file and read it back using BB (missing method in Root returning TASImage)
        view->GetGLViewer()->SavePictureUsingBB(viewFilename);
        TASImage *viewImg = new TASImage(viewFilename);
        
        //        tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingBB();
        
        
        // Second option is to use FBO instead of BB
        // This improves the quality of pictures in some specific cases
        // but is causes a bug (moving mouse over views makes them disappear
        // on new event being loaded
        
        //         if(index==0){
        //         tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(width3DView, height3DView);
        //         }
        //         else {
        //         tempImg = (TASImage*)view->GetGLViewer()->GetPictureUsingFBO(widthChildView, heightChildView);
        //         }
        //
        
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
    
    //draw ALICE Logo
    TASImage *aliceLogo = new TASImage(Form("%s/EVE/macros/alice_logo_big.png",gSystem->Getenv("ALICE_ROOT")));
    if(aliceLogo)
    {
        double ratio = 1434./1939.;
        aliceLogo->Scale(0.08*width,0.08*width/ratio);
        compositeImg->Merge(aliceLogo, "alphablend", 20, 20);
        delete aliceLogo;aliceLogo=0;
    }
    
    // draw info
    TTimeStamp ts(fESDEvent->GetTimeStamp());
    const char *runNumber = Form("Run:%d",fESDEvent->GetRunNumber());
    const char *timeStamp = Form("Timestamp:%s(UTC)",ts.AsString("s"));
    const char *system;
    if(strcmp(fESDEvent->GetBeamType(),"")!=0)
    {
        system = Form("Colliding system:%s",fESDEvent->GetBeamType());
    }
    else
    {
        system = "Colliding system: unknown";
    }
    const char *energy;
    if(fESDEvent->GetBeamEnergy()>=0.0000001)
    {
        energy = Form("Energy:%f",fESDEvent->GetBeamEnergy());
    }
    else
    {
        energy = "Energy: unknown";
    }
    int fontSize = 0.015*height;
    compositeImg->BeginPaint();
    compositeImg->DrawText(10, height-25-4*fontSize, runNumber, fontSize, "#BBBBBB", "FreeSansBold.otf");
    compositeImg->DrawText(10, height-20-3*fontSize, timeStamp, fontSize, "#BBBBBB", "FreeSansBold.otf");
    compositeImg->DrawText(10, height-15-2*fontSize, system,    fontSize, "#BBBBBB", "FreeSansBold.otf");
    compositeImg->DrawText(10, height-10-1*fontSize, energy,    fontSize, "#BBBBBB", "FreeSansBold.otf");
    compositeImg->EndPaint();
    
    // write composite image to disk
    TASImage *imgToSave = new TASImage(width,height);
    compositeImg->CopyArea(imgToSave, 0,0, width, height);
    
    // Save screenshot to file
    
    TGFileInfo fileinfo;
    const char *filetypes[] = {"PNG images", "*.png", 0, 0};
    fileinfo.fFileTypes = filetypes;
    fileinfo.fIniDir = StrDup(".");
    new TGFileDialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(),kFDOpen, &fileinfo);
    if(!fileinfo.fFilename)
    {
        cout<<"AliEveSaveViews::SaveWithDialog() -- couldn't get path from dialog window!!!"<<endl;
        return;
    }
    
    imgToSave->WriteImage(fileinfo.fFilename);
    
    if(compositeImg){delete compositeImg;compositeImg=0;}
    if(imgToSave){delete imgToSave;imgToSave=0;}
}

void AliEveSaveViews::BuildEventInfoString()
{
    if (AliEveEventManager::HasRawReader())
    {
        AliRawReader* rawReader = AliEveEventManager::AssertRawReader();
        if(!rawReader) return;
        fEventInfo.Form("Run: %d  Event#: %d (%s)",
                        rawReader->GetRunNumber(),
                        AliEveEventManager::CurrentEventId(),
                        AliRawEventHeaderBase::GetTypeName(rawReader->GetType())
                        );
        
        return;
    }
    else if (AliEveEventManager::HasESD())
    {
        AliESDEvent* esd =  AliEveEventManager::AssertESD();
        if(!esd)return;
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
        return;
    }
}

void AliEveSaveViews::BuildTriggerClassesStrings()
{
    ULong64_t mask = 1;
    int sw=0;
    
    AliESDEvent* esd =  AliEveEventManager::AssertESD();
    ULong64_t triggerMask = esd->GetTriggerMask();
    ULong64_t triggerMaskNext50 = esd->GetTriggerMaskNext50();
    
//    for(int clusterIter=0;clusterIter<fNumberOfClusters;clusterIter++)//loop over all clusters in run
//    {
        // get trigger classes for given cluster
    mask=1;
    for(int i=0;i<50;i++)
    {
        if(mask & triggerMask)
        {
            fTriggerClasses[sw]+=esd->GetESDRun()->GetTriggerClass(i);
//                fTriggerClasses[sw]+=Form("(%d)",fCluster[clusterIter]);
            fTriggerClasses[sw]+="   ";
            
            if(sw==0)sw=1;
            else if(sw==1)sw=2;
            else if(sw==2)sw=0;
        }
        if(mask & triggerMaskNext50)
        {
            fTriggerClasses[sw]+=esd->GetESDRun()->GetTriggerClass(i+50);
//                fTriggerClasses[sw]+=Form("(%d)",fCluster[clusterIter]);
            fTriggerClasses[sw]+="   ";
            
            if(sw==0)sw=1;
            else if(sw==1)sw=2;
            else if(sw==2)sw=0;
        }
        mask = mask<<1;
    }
//    }
}

void AliEveSaveViews::BuildClustersInfoString()
{
    vector<TString> clustersDescription;
    TString clustersInfo;
    ULong64_t mask = 1;
    
    for(int clusterIter=0;clusterIter<fNumberOfClusters;clusterIter++)//loop over all clusters in run
    {
        mask=1;
        for(int i=0;i<20;i++)
        {
            if(fInputDetectorMask[clusterIter] & mask)
            {
                clustersInfo+=AliDAQ::DetectorName(i);
                clustersInfo+=", ";
            }
            
            mask=mask<<1;
        }
        clustersDescription.push_back(clustersInfo);
    }
    
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



