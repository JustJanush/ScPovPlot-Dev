/****h* ScPovPlot3D/CommonDefs.inc
* PURPOSE
*   Common definitions of the whole system, sorry a little bit messy
*   TODO: cleanup and refactorization, esp. move coord system to dedicated file
*   |html <hr width=50% align="left">
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 4.0.0.0 (& have a nice time ;)           **
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
* HISTORY                  
*   2023-08-30 - #macro DebugMsg(_msg, _lvl) added, helper debug messenger
*   2021-06-09 
*      - minor improvements, cleaning, Changelog feat. dropped as not very useful;
*   2017-10-04
*      - txLCitronFT, txDCitronFT added;
*   2017-04-21
*      - txBlackFT introduced, 'emission' replaces obsolete 'ambient';
*   2017-04-21
*      - txDarkFT introduced, 'emission' replaces obsolete 'ambient';
*   2017-01-31
*    - formatting change 
*   2015-12-11
*    - extension: more image formats as wall background
*   2014-10-29, v. 3.1.0.2:
*    - added ChangeLog feature;
*    - changed InsertCartesianArrows_LDs behavior;
*  
*
****
  end of RoboDoc comment
****/

#ifndef(_CommonDefs_INC)
#declare _CommonDefs_INC="2021-06-09"; // last modification date
// common includes & definitions
#version 3.7;
#local _FileName   = "CommonDefs.INC";
#local _FileVerMaj = "4.0.0.1";
#local _FileVerMin = _CommonDefs_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")
// ============================================

// standard modules
#include "colors.inc"
#include "textures.inc"
#include "shapes.inc"
#include "glass.inc"
#include "metals.inc"
#include "strings.inc"

#ifndef (_CallStack ) // for tracking call stack xD
  #declare _CallStack = "Call stack:\n"  
#end

// Just 4 safety
#ifndef (Debug) #declare Debug=0; #end

// ScPovPlot modules
#include "scFinish.inc"

// helper macro = A^2
#macro SQR(A) (A)*(A) #end

#declare __Service = -1; // service levels: USE: #if (Debug < __Service)...
#declare __Stable  =  1; // usual messages: USE: #if (Debug > __Stable)...
#declare __RC      =  3; // RC stage
#declare __Beta    =  5; // beta
#declare __Alpha   =  7; // alpha mode
#declare __Init    =  9; // initial status

#declare _Filter   =  1; //
#declare _Transmit =  2; //

// Common color definitions //
// meaning of prefixes:
// tx = texture
// in = intrior
// mt = material
// colour postfixes
// F = filtered, FT filtered & translucent
#ifndef (Tiny) #declare Tiny=0.00001; #end
#declare endl="\n";
/*********************[  some textures  ]*******************************/
/*********************[ pure flat color ]*******************************/
#declare txRed   = texture {pigment { color rgb<0.8, 0.2, 0.2> }finish { ambient 0.2 diffuse 0.5 }}
#declare txGreen = texture {pigment { color rgb<0.2, 0.8, 0.2> }finish { ambient 0.2 diffuse 0.5 }}
#declare txBlue  = texture {pigment { color rgb<0.2, 0.2, 0.8> }finish { ambient 0.4 diffuse 0.7 }}

/*********************[ filtered color ]*******************************/
#declare txRedF   = texture {pigment { color rgbf<0.8, 0.2, 0.2, .8> }finish { ambient 0.2 diffuse 0.5 }}
#declare txGreenF = texture {pigment { color rgbf<0.2, 0.8, 0.2, .8> }finish { ambient 0.2 diffuse 0.5 }}
#declare txBlueF  = texture {pigment { color rgbf<0.2, 0.2, 0.8, .8> }finish { ambient 0.2 diffuse 0.5 }}

/********************[ filt&transp color ]***************************/
#declare txRedFT   = texture {pigment { color rgbft<0.8, 0.2, 0.2, .7, .3> }finish { ambient 0.2 diffuse 0.5 }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}
#declare txGreenFT = texture {pigment { color rgbft<0.2, 0.8, 0.2, .7, .3> }finish { ambient 0.2 diffuse 0.5 }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}
#declare txBlueFT  = texture {pigment { color rgbft<0.2, 0.2, 0.8, .7, .3> }finish { ambient 0.2 diffuse 0.5 }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}
#declare txWhiteFT = texture {pigment { color rgbft<0.4, 0.45, 0.4, .7, .3> }
                              finish  { emission .1 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txDarkFT = texture {pigment { color rgbft<0.1, 0.1, 0.1, .9, .1> }
                              finish  { emission .1 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txBlackFT = texture {pigment { color rgbft<0.1, 0.1, 0.1, .4, .5> }
                              finish  { emission .1 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}
/********************[ filt&transp color ]***************************/
#declare txLghtRedFT   = texture {pigment { color rgbft<.6, 0.4, 0.4, .7, .3> } finish { ambient 0.2 diffuse 0.5 }
                                  finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLghtGreenFT = texture {pigment { color rgbft<0.3, .5, 0.3, .7, .3> } finish { ambient 0.2 diffuse 0.5 }
                                  finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLghtBlueFT  = texture {pigment { color rgbft<0.4, 0.4, 0.6, .7, .3> }
                                  finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}


#declare txGlass = texture {pigment { color red .75 green .75 blue .75 filter 0.95 }
                            finish  { ambient 0.3 diffuse 0.7 reflection 0.0050 phong 0.2 phong_size 90 }}

#declare txMica = texture{pigment{color Mica}};// finish{scDullMirror}};

#declare txMicaFF = texture{pigment{color Mica}
                            finish{scDullMirror}};

#declare txDGreenFT = texture {pigment { color rgbft<0.0, 0.4, 0.0, .7, .3> }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLGreenFT = texture {pigment { color rgbft<0.0, 0.5, 0.0, .7, .3> }
                              finish  { ambient 0.3 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txDBlueFT = texture {pigment { color rgbft<0.0, 0.0, 0.4, .7, .3> }
                              finish  { ambient 0.3 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLBlueFT = texture {pigment { color rgbft<0.0, 0.0, 0.5, .7, .3> }
                              finish  { ambient 0.3 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txDPlumFT = texture {pigment { color rgbft<0.3, 0.0, 0.3, .7, .3> }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLPlumFT = texture {pigment { color rgbft<0.5, 0.0, 0.4, .7, .3> }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLPlumFT2 = texture {pigment { color rgbft<0.8, 0.0, 0.7, .9, .1> }
                              finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txLCitronFT = texture {pigment { color rgbft<1.0, 1.0, 0.0, .7, .3> }
                                finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txDCitronFT = texture {pigment { color rgbft<0.7, 0.7, 0.0, .7, .3> }
                                finish  { ambient 0.2 diffuse 0.5  reflection {0.1, 1.0   fresnel on } conserve_energy }}

#declare txAluminium = texture{ Brushed_Aluminum finish { diffuse 0.9 phong 1 } scale 0.5  } 

#declare txMarmurek  = texture{pigment{ White_Marble } normal { bumps 0.6 scale 0.05} finish { diffuse 0.9 phong 1 } scale 0.2} 

/*********************\  some interiors  \*******************************/
#declare inGlass  =  interior {  ior 1.5   fade_distance .50  fade_power 2  }
#declare inGlass0 =  interior {  ior 2.5   dispersion 1.1 dispersion_samples 11 fade_distance .50  fade_power 2  }
#declare inGlass1 =  interior {  ior 2.5   fade_distance .50  fade_power 2  }
#declare inGlass2 =  interior {  ior 2.5   dispersion 1.1 dispersion_samples 11 fade_distance .50  fade_power 2  }

/********************[ some materials ]*************************/
#declare mtGlass=  // Glass material, mainly for testing, sorry
material {
  texture {
    pigment {rgbt 1}
    finish {
      ambient 0.0
      diffuse 0.05
      specular 0.6
      roughness 0.005
      reflection {
        0.1, 1.0
        fresnel on
      }
      conserve_energy
    }
  }
  interior {
    ior 1.5
    fade_power 1001
    fade_distance 0.9
    fade_color <0.5,0.8,0.6>
  }
}

#declare mtGlassA =  // Glass material, ambient set to .7
material {
  texture {
    pigment {rgbt 1}
    finish {
      ambient .7
      diffuse 0.05
      specular 0.6
      roughness 0.005
      reflection {
        0.1, 1.0
        fresnel on
      }
      conserve_energy
    }
  }
  interior {
    ior 1.5
    fade_power 1001
    fade_distance 0.9
    fade_color <0.5,0.8,0.6>
  }
}
//#end

//****** some default settings, mainly for Histograms and CoordsSys *******//
#declare tmpColour   = rgb <0,0,1>; // well, I like blue colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.1>; // ..transparent a little..
#declare tmpColour2  = color rgbf<1.5, 1.3, 1.2, 0.3>;
#declare tmpInterior = interior{inGlass};
#declare tmpTexture  = txBlue;
#declare tmpMaterial = mtGlass;
//=========================================================================

#macro Procent(_dY, _dZ, _R)//(4, 7, .5)
    union{
       sphere{ 0, _R}
       sphere{ <_dY, _dZ, 0>, _R}
       sphere{ <0, _dZ, 0>, _R}
       sphere{ <_dY, 0, 0>, _R}
       cylinder{0, <_dY, _dZ, 0>, _R }
    }
#end

#local   __ef      = 1.01; // internal scaling factor, influences background images
#declare ImgGamma  = 2.20; // 0=>srgb,
#declare ImgFilter = 0.00; // inactive
#declare ImgPower  = 0.50; // obsolete, one of 'filter' XOR 'transmit' amount
#declare __ImgFilterSet   = no;  // inactive
#declare __ImgFilterPower = 1.0; // default

#declare __ImgTransmitSet   = no;   // default -inactive
#declare __ImgTransmitPower = 0.0;  // default

#declare __MattSet     = yes;    // default - active
#declare __MattTexture = texture{ pigment{ color rgb .5} finish{ emission 1}} // matt bck image texture
#declare __ImageType   = "jpeg"; // default image type

#declare ImgInter   = 2;
#declare ImgReflect = 0;
#declare ImgFinish  = finish {reflection {ImgReflect metallic} diffuse .5 ambient 0.7};// default value

#declare XYimage   = "";
#declare XZimage   = "";
#declare YZimage   = "";
#declare NoTiles   = false; // constant
#declare Tiles     = true;  // constant
#declare Effects   = false;

#declare TilXYimage = false;
#declare TilXZimage = false;
#declare TilYZimage = false;

#declare DeclareLevel = 0; // mainly used in "Histogram.inc", "CoordsSys.inc"
// DeclareLevel determines what is defined:
//  0 = colour only
//  2 = also texture
//  4 = complete material
//  8 = full bar definition, including material, shape and animation
// 16 = full bar definition by macro
// this value is valid until next "Set...()" declaration is met
//********************************************/

#macro SetColor( FullColour )
    #declare tmpColour = FullColour ;
    #declare DeclareLevel = 0;
#end

#macro SetRGBColor(RR, GG, BB)
    #declare tmpColour = rgb<RR, GG, BB >;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor( RR, GG, BB, FF, TTT )
    #declare tmpColour = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_1( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour1 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetRGBFTColor_2( RR, GG, BB, FF, TTT ) // for hollow bar
    #declare tmpColour2 = rgbft<RR, GG, BB, FF, TTT>;
    #declare DeclareLevel = 0;
#end

#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

#macro SetInterior( tInt )       // valid only together with texture defintions
    #declare tmpInterior = interior {tInt};
    #declare DeclareLevel = 2;
#end

/****f* CommonDefs.inc/SetMaterial
* PURPOSE
*   Sets material{} & DeclareLevel at '4'
* SYNOPSIS
*/
#macro SetMaterial( _FMat )
/*
* INPUTS
*   materialdef _FMat - contains finish {} section for background images
* SEE ALSO
******/
    #declare tmpMaterial  = _FMat;
    #declare DeclareLevel =     4;
#end

/****f* CommonDefs.inc/SetImgFinish
* PURPOSE
*   Sets "finish" section
* SYNOPSIS
*/
#macro SetImgFinish (_Fin)           // finish identifier
/*
* INPUTS
*   finishdef _Fin - contains finish {} section for background images
* SEE ALSO
*   SetImageGamma()
*   SetImageFilter()
******/
    #declare ImgFinish = _Fin;
#end

/****f* CommonDefs.inc/SetImageGamma [ SetImageFilter, SetImgFinish ]
* PURPOSE
*   Sets gamma value for background image
* SYNOPSIS
*/
#macro SetImageGamma (_Val)         // '0' means 'srgb';
/*
* INPUTS
*   float _Val - gamma value for image given
******/
   #declare ImgGamma = _Val;
#end

/****f* CommonDefs.inc/SetImagePower  [SetImgFilter, SetImageGamma, SetImgFinish]
* PURPOSE
*   Sets VALUE of "filter" XOR "transmit". Obsolete, use SetImgFilter OR/AND SetImgTransmit instead
* SYNOPSIS
*/
#macro SetImagePower (_Val)         // float
/*
* INPUTS
*   float _Val - amount of "transmit" (2) XOR "filter" (1) for image given
******/
   #declare ImgPower = _Val;
#end


/****f* CommonDefs.inc/SetImageFilter  [ SetImgFilter, SetImageGamma, SetImgFinish ] 
* PURPOSE
*   Sets filter XOR transmit value for image. Obsolete, use SetImgFilter instead
* SYNOPSIS
*/
#macro SetImageFilter (_Val)        // 1 or 2
/*
* INPUTS
*   pinteger _Val - filtering type: "transmit" (2) XOR "filter" (1) for image given
******/
   #declare ImgFilter = _Val;
#end

/****f* CommonDefs.inc/SetImgFilter, UnSetImgFilter, SetImgTransmit, UnSetImgTransmit
* PURPOSE
*   Sets amount of light filtered/changed
* SYNOPSIS
*/
#macro SetImgFilter (_V)
/*
* INPUTS
*   float _V - amount of filtered light/filter power, if _V <0 restores previous, else sets new value
******/
    #declare __ImgFilterSet = yes;  // activate
    #if ( _V >= 0 )
       #declare __ImgFilterPower = _V; // default
    #end
#end

/****f* CommonDefs.inc/UnSetImgFilter
* PURPOSE
*   Deactivates filtering stack
* SYNOPSIS
*/
#macro UnSetImgFilter(_V)
/*
* INPUTS
*   float pwr - amount of filtered light/filter power
******/
    #declare __ImgFilterSet = no;   // deactivate
#end

/****f* CommonDefs.inc/SetImgTransmit, UnSetImgTransmit, SetImgFilter, UnSetImgFilter
* PURPOSE
*   Sets amount of light transmitted as unchanged
* SYNOPSIS
*/
#macro SetImgTransmit (_V)
/*
* INPUTS
*   float pwr - amount of transmitted light/transmit power, if _V <0 restores previous, else sets new value
******/
    #declare __ImgTransmitSet   = yes;  // activate
    #if ( _V >= 0 )
        #declare __ImgTransmitPower = _V;
    #end
#end

/****f* CommonDefs.inc/UnSetImgTransmit, SetImgTransmit, SetImgFilter, UnSetImgFilter
* PURPOSE
*   Deactivates transmit stack
* SYNOPSIS
*/
#macro UnSetImgTransmit()
/*
* INPUTS
*   no inputs
******/
    #declare __ImgFilterSet = no;  // deactivate
#end

/****f* CommonDefs.inc/SetMatt, UnSetMatt, UnSetImgTransmit, SetImgFilter, UnSetImgFilter
* PURPOSE
*   Sets texture for image background matt
* SYNOPSIS
*/
#macro SetMatt (_Tx)
/*
* INPUTS
*   texturedef Tx -texture for background image matt
******/
    #declare __MattSet = yes;  // activate
    #declare __MattTexture = _Tx;
#end

/****f* CommonDefs.inc/UnSetMatt, SetMatt, UnSetImgTransmit, SetImgTransmit, SetImgFilter, UnSetImgFilter
* PURPOSE
*   Deactivates texture for image background matt
* SYNOPSIS
*/
#macro UnSetMatt()
/*
* INPUTS
*   no inputs
******/
    #declare __MattSet = no;  // deactivate
#end
//=================================================



/****f* CommonDefs.inc/ImageType
* PURPOSE
*   Deactivates texture for image background matt
* SYNOPSIS
*/
#macro ImageType (_Imgx)
/*
* INPUTS
*   string_fn _Img - image file name with extension
******/
   #local len=strlen(_Imgx);
   #local idx=len;
   #while( idx>0 )
       #local _c = substr(_Imgx, idx, 1);
       #if (_c=".") #break #end   // last 'n' chars until 'dot' is found
       #local idx=idx-1;
   #end
// example "Wawel.jpg" len = 9,  dot at idx=6
   #local _ext = substr(_Imgx, idx+1, len-idx);
   #if (Debug)  #debug concat(endl, "CommonDefs.inc: [ext=", _ext, "]", endl)  #end   // exr | gif | hdr | iff | jpeg | pgm | png | ppm | sys | tga | tiff
   #if (_ext =  "jpg")  #declare __ImageType = "jpeg"; #end
   #if (_ext = "jpeg")  #declare __ImageType = "jpeg"; #end
   #if (_ext =  "png")  #declare __ImageType =  "png"; #end
   #if (_ext = "tiff")  #declare __ImageType = "tiff"; #end
   #if (_ext =  "tif")  #declare __ImageType = "tiff"; #end
   #if (_ext =  "gif")  #declare __ImageType =  "gif"; #end
   #if (_ext =  "exr")  #declare __ImageType =  "exr"; #end
   #if (_ext =  "hdr")  #declare __ImageType =  "hdr"; #end
   #if (_ext =  "iff")  #declare __ImageType =  "iff"; #end
   #if (_ext =  "pgm")  #declare __ImageType =  "pgm"; #end
   #if (_ext =  "ppm")  #declare __ImageType =  "ppm"; #end
   #if (_ext =  "tga")  #declare __ImageType =  "tga"; #end
   #if (_ext =  _Imgx)  #declare __ImageType =  "sys"; #end   // no dot_extension given
#end

#macro SetImgInterpolation(Value)   // 2, 3 or 4
   #declare ImgInter = Value;
#end

#macro SetImgReflection(Value)      // 0 to 1
   #declare ImgReflect=Value;
   #declare ImgFinish = finish {reflection {ImgReflect metallic} diffuse .5 ambient 0.7};// default value
#end

#macro SetImageXY(_Img, _T)
   #declare XYimage = _Img;
   #declare TilXYimage = _T;
#end

#macro SetImageXZ(_Img, _T)
   #declare XZimage = _Img;
   #declare TilXZimage = _T;
#end

#macro SetImageYZ(_Img, _T)
   #declare YZimage = _Img;
   #declare TilYZimage = _T;
#end

/****f* CommonDefs.inc/Sfere
* PURPOSE
*  computes residual f(x,y,z) = x^2+y^2+z^2 - _a^2, thus for threshold _a == 0; 
*  isosurface{} object produces sphere of radii '_a' (check this)
*
* SYNOPSIS
*/
#declare Sfere = function (x,y,z,_a) {pow(x,2) + pow(y,2) + pow(z,2) - pow(_a,2)};
/*
* INPUTS
*  x,y,z - float: just three floats, may be some point coordinates
*  _a    - float: just float or one of right triangle sides
* OUTPUTS
*  float - square of third right triangle side
* SEE ALSO
*  None
*
******/

/****f* CommonDefs.inc/Factorial
* PURPOSE
*  computes factorial of the number 'C'
*  Factorial(c)=c!
*
* SYNOPSIS
*/
#declare Factorial = function(_c) { prod(i, 1, _c, i) }
/*
* INPUTS
*  float or integer - factorial parameter,
* OUTPUTS
*  float - factorial of _c if _c is whole number, maybe factorial if float
* SEE ALSO
*  None
*
******/


/****f* CommonDefs.inc/DebugMsg
* PURPOSE
*  Handy debug messenger, turns on at given Debugging/verbose level
*
* SYNOPSIS
*/
#macro DebugMsg(_msg, _lvl)
/*
* INPUTS
*  string _msg  - debugging message - raw text, no enhancement possible;
*  integer _lvl - debugging level trigger
* OUTPUTS
*  none
* SEE ALSO
*  None
*
******/
  #if (_lvl = 0) 
    #debug _msg  
  #else 
    #if (bitwise_and( _lvl,Debug))
       #debug _msg
    #end      
  #end   
#end // macro

#else
 DebugMsg("Module Potential recalled\n",1)
 #ifndef (_CallStack )
   #declare _CallStack = "Call stack:\n"  
 #end
 #declare _CallStack = concat(_CallStack, _ModuleDescription,"\n")

#end // #ifndef(_CommonDefs_INC)