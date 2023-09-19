/****h* ScPovPlot3D/Histogram.inc
* PURPOSE
*   Old, well known templates, no improvements yet, sorry. But in the next edition
*   I plan to add some analytics, ie data to be read from raw file
*     image:./imgs/ShapeTransparent.jpg
*     image:./imgs/HistogramS.png
*     image:./imgs/Histogram2S.png
*
*     |html <hr width=50% align="left">
*
*     Fig.[Histogram] Examples of histogram charts         
*     |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.1.0.9 (& have a nice time ;)             **
*     *********************************************************
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo@agh.edu.pl, janusz.opila@gmail.com
*   Dept. of Applied Informatics
*   AGH University of Science & Technology, Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
*  
* HISTORY
*  2017-02-02, 3.1.0.7 - X-axis description impvnts
*  2018-08-30, 3.1.0.8 - Vertical axis description bug fixed
*  2019-02-27, 3.1.0.9 - reuse of DscrColAxe table enabled, DrawColDscr adjusted
****
  end of RoboDoc comment
****/

#ifndef  (Histogram_INC)
#declare Histogram_INC="2019-02-27";
#version 3.7;

#local _FileName   = "Histogram.inc";
#local _FileVerMaj = "3.1.0.9";
#local _FileVerMin = Histogram_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

//*******************************************//
//******* important include files ***********//
#include "CommonDefs.inc"
#include "CoordsSys.inc"
#include "Cameras.inc"
#include "BarShapes.inc"
#include "TextExt.inc"

//*********** some default values ****************/
#declare deltaX=1.0;      // x distance between bars
#declare deltaZ=1.0;      // z distance between bars
#declare deltaXext=0.5;   // external x distance (before & after) bars
#declare deltaZext=0.5;   // external z distance (before & after) bars
// x-Z logical bar's footprint
#declare Xside=1.0;
#declare Zside=1.0;

#declare BaseX=  deltaXext+Xside/2;   // initial x center of bar
#declare BaseZ= -deltaZext-Zside/2;   // initial z center of bar

#declare SceneMinY= 0.0;     // initial setting, correct it later, please! no bars at the time
#declare SceneMaxY= 0.0;     // no bars at the time

#declare SceneMinX= 0.0;     // no bar at the time
#declare SceneMinZ= 0.0;     // no bar at the time

#declare SceneMaxX= 2*deltaXext;    // no bar at the time
#declare SceneMaxZ=-2*deltaZext;    // no bar at the time

#declare MaxY    = 0.0;
#declare DscMaxY = 0.0;      // MaxY for description only
#declare YScaleFactor = 1.0; // exaggerates vertical dimension

#declare LettrSize  =  0.8;
#declare LettrDepth =  0.2;
#declare LettrBase  = -0.7;
#declare DscrAngle  =  0.0;
#declare _VDAngle   = 90.0;
#declare NumDscrLgh =    4;  // Total length of number
#declare NumDscrDig =    1;  // Digits after decimal point
#declare ShiftXY    = -0.4;  // float: move vertical description labels by that amount, can change it later on..
#declare _dzShift   = -0.1;  // float: adjustment along z-axis
#declare _Prefix    =   "";  // string, usually label/number prefix   
#declare _Postfix   =   "";  // string, usually label/number prefix   

#macro SetPrefix(prf)               
   #declare _Prefix = prf;
#end

#macro SetPostfix(pst)
   #declare _Postfix = pst;                 
#end                    

#declare NRow=1;             // at least one row
#declare NCol=0;
#declare MaxCol=0;           // highest number of cols in any row

//*********** initial scene attribues **********//
#declare Effects=false;
#declare Photons=false;

/// initial default safe value
/****f* Histogram.inc/Set_TextFont, _TextFont
* PURPOSE
*  Defines font for textual elements of the chart
*    WARNING: Font must exist in the host operating system
*    default setting "Arial.ttf" should be safe on Windows, as well as on some GNU/Linux distros
* SYNOPSIS
*/
#declare _TextFont = "Arial.ttf";
#macro Set_TextFont (_TxtFnt)
/*
 * INPUTS
 *  _TxtFnt - string containing name of the file containing given font, ex. "Arial.ttf". Sets internal local variable _TextFont
 * SEE ALSO 
 *  Set_TFont ()
 ****/
    #declare _TextFont = _TxtFnt;
#end

/****f* Histogram.inc/Set_ShiftXY, ShiftXY
* PURPOSE
*  moves description labels of vertical axe along X-axis by that amount, can change it later on..
* SYNOPSIS
*/
#macro Set_ShiftXY(shft)
/*
 * INPUTS
 *  shft - float - distance in POVRay units along X_P axis
 * SEE ALSO
 *  empty
 ****/
    #declare ShiftXY = shft;
#end


/****f* Histogram.inc/Set_ShiftZ, _dzShift
* PURPOSE
*  moves description labels (horizontal only) by that amount along z-axis, can change it later on..
* SYNOPSIS
*/
#macro Set_ShiftZ(shft)
/*
 * INPUTS
 *  shft - float - distance in POVRay units along Z_P axis
 * SEE ALSO
 *  empty
 ****/
  #declare _dzShift   = shft;
#end                                      

/****f* Histogram.inc/ReSet_dX_dY
* PURPOSE
*  sets distance between bars along X and Z! axes
* SYNOPSIS
*/
#macro ReSet_dX_dY(_dx, _dy)
/*
 * INPUTS
 *  _dx, _dy - float, float - distance in POVRay units
 * SEE ALSO
 *  empty
 ****/
    #declare deltaX=_dx;
    #declare deltaZ=_dy;
#end

// sets external distance (before & after) bars along X and Z! axes
// apply only once!!
#macro Set_ext_dX_dY(_dx, _dz)
   #ifndef  (Set_ext_dX_dY_USED)
       #declare Plot3DHistogram=1;
       #declare deltaXext=  abs(_dx);  // always positive!!
       #declare deltaZext=  abs(_dz);  // always positive!!
       #declare BaseX=  deltaXext + Xside/2; // initial x bar center
       #declare BaseZ= -deltaZext - Zside/2; // initial z bar center/lefthanded cartesian!/
       #declare Set_ext_dX_dY_USED=1;
   #end // #ifndef
#end

// sets distance between bars along X and Z! axes
// apply only once!!
#macro Set_dX_dY(_dx, _dz)
   #ifndef  (Set_dX_dY_USED)
      #declare Plot3DHistogram=1;
      #declare deltaX=_dx;
      #declare deltaZ=_dz;
      #declare Set_dX_dY_USED=1;
   #end // #ifndef
#end

// sets logical footprint of bars
// apply only once, redeclaration blocked!!
#macro Set_Xside_Yside(xs, zs)
  #ifndef  (Set_Xs_Ys_USED)
    #declare Xside = abs(xs); // always positive
    #declare Zside = abs(zs); // always positive
    #declare BaseX =  deltaXext + Xside/2; // initial x bar center
    #declare BaseZ = -deltaZext - Zside/2; // initial z bar center/lefthanded cartesian!/
    #declare Set_Xs_Ys_USED=1;
  #end // #ifndef
#end

#declare tmpObject = StandardBar; // simple rectangular prism/ perpendicular

// #declare DeclareLevel = 0; defined in CommonDefs

// obvious stuff
#macro SetXDscOffset(_Off)
   #declare ShiftXY=_Off; // float:
#end

#macro SetLettrSize(ls)
    #declare LettrSize = ls;
#end
#macro SetLettrDepth(ls)
    #declare LettrDepth= ls;
#end

#macro SetLettrBase(ls)
    #declare LettrBase =ls;
#end

#macro SetDscrAngle(ls)
    #declare DscrAngle=ls;
#end                      

#macro SetVDscrAngle(_an) // vertical rotation angle
  #declare _VDAngle = _an;
#end

/// default setting for safety
#declare BgPicture="";
/// sets local URL of background picture (as path/filename in local filesystem)
#macro SetBgPicture(_Pct)
  #declare BgPicture = _Pct;
#end

#macro TurnPhotonsDown()
    #declare Photons=false;
#end

#macro TurnPhotonsUp()
    #declare Photons=true;
#end

#macro SetEffectsOn()
    #declare Effects=true;
#end

#macro SetEffectsOff()
    #declare Effects=false;
#end


#macro SetBarObject( obj )      // full object definition without texture, interior,...
    #declare tmpObject=obj;     // doesn't change declaration level
#end

#macro SetBarObjectFully( obj ) // full object definition (+texture +interior +...)
    #declare tmpObject=obj;
    #declare DeclareLevel = 8;
#end

#macro SetBarObjectFullyNSc( obj ) // full object definition (YYY, +texture +interior +...)
    #declare tmpObject=obj;
    #declare DeclareLevel = 16;
#end

#macro SetYScaleFactor(YSF)     // vertical scale exaggeration
    #declare YScaleFactor = YSF;
#end

#macro AdjustMaxY(Y) // Overrides automatically evaluated number of MaxY
    #declare DscMaxY = Y;
    #declare MaxY= DscMaxY*YScaleFactor;        
    #declare SceneMaxY = MaxY;
#end

#macro NextRow()
  #declare NRow  = NRow+1;
  #declare BaseZ = -(NRow-1)*(Zside+deltaZ)-Zside/2-deltaZext;
  #declare BaseX = deltaXext+Xside/2; // reset to first position
  #declare NCol  = 0;

// ie. it is more negative..
  #declare SceneMaxZ=min(SceneMaxZ, -NRow*Zside-2*deltaZext);
#end            


/// TODO: how to add another histogram aside?
/**************************************/
#macro SetNumFormat(L, D)
    #declare NumDscrLgh=L;
    #declare NumDscrDig=D;
#end

#macro DrawVrtDescr(_NVrt, _Texture)
  #declare _NVrt=_NVrt+1;
  #declare DscrVrtAxe = array[_NVrt];     // description along Y-axis (ie vertical axe)
  #if (_NVrt>1)
    #declare VrtDlt = DscMaxY/(_NVrt-1);
    #declare Cnt=0;
    #while (Cnt<_NVrt)
       #declare DscrVrtAxe[Cnt]= Cnt*VrtDlt;
       #declare Cnt=Cnt+1;
    #end

    #declare Cnt=0;
    #while (Cnt<_NVrt)   // Y-axe AKA Cartesian 'Z'
      text{// add prefix & suffix option
         ttf _TextFont, concat(_Prefix,str( DscrVrtAxe[Cnt], NumDscrLgh, NumDscrDig),_Postfix), LettrDepth, 0 texture{_Texture}
         scale LettrSize
         rotate ((camera_z>0?180:0)+DscrAngle)*y
         translate<(camera_x>0?SceneMinX-sgn(camera_z)*ShiftXY:SceneMaxX-sgn(camera_z)*ShiftXY),
                    YScaleFactor*(DscrVrtAxe[Cnt])-LettrSize/2.0,  // 2018-08-30
                    (camera_z<0?1.05*(SceneMaxZ-SceneMinZ):(SceneMinZ-0.05*(SceneMaxZ-SceneMinZ)))>
      }
    #declare Cnt=Cnt+1;
    #end

  #end //#if
#end

#macro InitDscrColAxe()
  #ifndef (DscrColAxe)
    #if (MaxCol>0)
        #declare DscrColAxe = array[MaxCol];       // description along X-axis (ie column axe)
        #for(i, 0, MaxCol-1)
           #declare DscrColAxe[i]=""; 
        #end
    #else
        #debug "Can\'t init Col-axis description array - add some bars first\n"
    #end             
  #end  
#end                 

/**************************************/
#macro AddColDescr(Dscr)
  #ifndef (ColCnt)
    #declare ColCnt=0;
  #else
    #declare ColCnt=ColCnt+1;
  #end
    #if (ColCnt<NCol)
      #declare DscrColAxe[ColCnt] = concat(_Prefix,Dscr,_Postfix);
    #else
      #warning concat("==>Too many descriptions for cols: \"", Dscr, "\" rejected\n")
    #end
#end
/**************************************/
//Znn
#macro DrawColDscr(_Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base

  #declare xPos=Xside/2.0+deltaXext+ShiftXY; //  additional shift along X axis

  Set_TFont( _TextFont  )     //n
  SetLDpth( _LettrDepth )     // set depth of letters (z-axis size) by SetLDpth() macro
  #declare _ZShift  =.05*SceneMaxZ-_dzShift; // as standard 5% shift, but might be overriden by _dzShift setting
  
  #declare Cnt = 0;
  #while(Cnt<NCol)
//    #declare TxtTmp = object { text{ttf _FF DscrColAxe[Cnt] 1 0} rotate _VDAngle*x  scale _LettrSize};   //n 
//    #local shft = (max_extent(TxtTmp).x-min_extent(TxtTmp).x)/2.0;
//    #debug concat("MaxX=", str(max_extent(TxtTmp).x, 5, 3)," ","MinX=", str(min_extent(TxtTmp).x, 5, 3), "\n")
//    #debug concat("Shift=", str(shft, 5, 3),"\n")

    #declare TxtTmp = object { Text(DscrColAxe[Cnt]) rotate _VDAngle*x  };//scale _LettrSize};   //n 
    #local shftX = _LettrSize*GetTextExtent()/2.0;
    #debug concat("ShiftX=", str(shftX, 5, 3),"\n")

     object { TxtTmp
        texture{_Txtr}     //n
        scale LettrSize
        rotate DscrAngle*y //n
        translate<xPos-shftX, _LettrBase, SceneMaxZ+_ZShift>
     }
     #declare xPos=xPos+Xside+deltaX;
     #declare Cnt=Cnt+1;
  #end                
  #undef ColCnt  
#end
/**************************************/
#macro InitDscrRowAxe()
    #declare DscrRowAxe = array[NRow];     // description along Z-axis (ie row axe)
#end

#macro AddRowDescr(Dscr)
  #ifndef (RowCnt)
    #declare RowCnt=0;
  #else
    #declare RowCnt=RowCnt+1;
  #end
  #if (RowCnt<NRow)
    #declare DscrRowAxe[RowCnt] = concat(_Prefix,Dscr,_Postfix);
  #else
    #warning concat("==>Too many descriptions for rows: \"", Dscr, "\" rejected\n")
  #end
#end
/**************************************/

//Znn
/**************************************/
#macro DrawRowDscr(_Txtr, _LettrBase, _LettrSize, _LettrDepth) // Texture, Letter base
//  #declare ZSide = -NRow*BaseZL;
  #declare ZPos = -Zside/2-deltaZext-_dzShift;
  Set_TFont( _TextFont  )     //n
  SetLDpth( _LettrDepth )     // set depth of letters (z-axis size) by SetLDpth() macro
  #declare _XShift  =.05*SceneMaxX+ShiftXY;

  #declare Cnt = 0;
  #while(Cnt<NRow)
     #declare TxtTmp = object { Text(DscrRowAxe[Cnt])  rotate _VDAngle*x scale _LettrSize }
     #local shft = (max_extent(TxtTmp)+min_extent(TxtTmp))/2.0;

     object { TxtTmp 
         texture{_Txtr}     //n

         translate -shft    // move to origin
         rotate DscrAngle*y // Rotate around centre of the object
         translate  shft    // ... then transfer back          
         translate<SceneMaxX+_XShift, _LettrBase, ZPos>
     }

    #declare ZPos=ZPos-Zside-deltaZ;  // usual stuff
    #declare Cnt=Cnt+1;
  #end
#end
/**************************************/
// adding bars

// Simplified front end to AddBarFullUni()
// preferred method!!!
#macro AddBar(YYY)
   AddBarFullUni(YYY, Xside, Zside)
#end

#macro AddBarUni(obj, YYY)  // inserts object 'obj' as a bar
   SetObject(obj)
   AddBarFullUni(YYY, Xside, Zside)
#end

/********************************************************************/
// this is (almost) universal swiss knife
// prepend it with Set##Color(), SetTexture|Material|Interior
// and 'obj' definition like this exemplary one
// 'obj' should be positioned on Z-X plane
// and with vertical quasi-symmetry axis aligned with OY axis
// USAGE:
// #declare MyOwnBar=cylinder{0,<0,1,0>,1};
// AddBarFullUni(MyOwnBar,3,1,1)
#macro AddBarFullUni(YYY, _dX, _dZ)
   #declare YYY=YScaleFactor*YYY;

   object {tmpObject
   #switch (DeclareLevel)
     #case(0)  // apparently colour is defined
          material{ texture { pigment{tmpColour}
          #if (Effects)
                 normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
          #end
             } // end of texture
          } // end of material -------------------
           #break
     #case(2) // apparently texture is defined
          material{
                texture { tmpTexture } // end of texture
                #if (Effects)
                   normal { bumps 0.5 scale 0.05} finish { diffuse 0.9 phong 1 }
                #end
                interior{ tmpInterior } // end of interior
            } // end of material -------------------
           #break
     #case(4) // apparently material has been set
          material{ tmpMaterial }
          #break

     #case(8) // apparently full object definition was set
              // nothing to do!
          #break

     #case(16) // apparently full object definition was set
               // nothing to do!
          #break

     #else #break
   #end

   #if (Photons)
           photons {  // photons block
              target 1
              refraction on
              reflection on
              collect on
           }
   #end
   #if (DeclareLevel>=8)
//      scale <_dX, 1.0, _dZ>  // no scale along Y-axis, object just did it on its own !!
   #else
      scale <_dX, YYY, _dZ>
   #end
   translate <BaseX, 0, BaseZ>
}

// Adjust coordinates
  #declare BaseX = BaseX + _dX + deltaX;
  #declare NCol=NCol+1;
  #if (NCol>MaxCol)              // readjust number of cols
     #declare MaxCol=NCol;
  #end

  #declare SceneMaxX =  MaxCol*Xside+ (MaxCol-1)*deltaX + 2*deltaXext;
  #declare SceneMaxZ = -NRow*Zside  - (NRow-1)*deltaZ   - 2*deltaZext;

  #if (YYY > MaxY)
    #declare MaxY=YYY;
    #declare DscMaxY=MaxY/YScaleFactor;
  #end
  #declare SceneMaxY = max(SceneMaxY, MaxY);

#end

/********************************************************************/
#end // #ifndef Plot3DHistogram_INC
