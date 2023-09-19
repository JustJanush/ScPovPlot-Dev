/****h* ScPovPlot3D/BPatchSurf.inc
* PURPOSE
*   New templates for drawing smoothed by Bezier spline surfaces:
*     image:./imgs/MonoPatchS2.png
*     image:./imgs/GriddnColorMapSgray.jpg
*     image:./imgs/MultiPatchS1.jpg
*     image:./imgs/MonoPatchS1.png       
*
*   Fig.[BPatchSurf]. New, 3D surface modelling system. There is still a lot TODO: more automation and so on
*   |html <hr width=50% align="left">
* VERSION
*  3.1.0.3, tested on PovRay 3.7.
* HISTORY
*  3.1.0.3 replaced in line 1085: <xx,yy,zz> -> <xx,yy,VScale*zz+VShift>
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics, 
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland
*   Maintained by Janusz Opi³a Ph.D.
*   Homepage: http://scpovplot3d.sourceforge.net   
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
* HISTORY                            
* 2021-06-29 - raw points limits detection in RawDataSurface() macro
* 2021-06-10 - minor improvements in comments
*
****
  end of RoboDoc comment
****/

#ifndef(BPatchSurf_INC)
#declare BPatchSurf_INC = "2021-06-29";
#version 3.7;

#declare _FileName   = "BPatchSurf.inc";
#declare _FileVerMaj = "4.0.0.4";
#declare _FileVerMin = BPatchSurf_INC;
#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

// Just 4 safety
#ifndef (Debug) #declare Debug=0; #end

#include "ColorMaps.inc"

/****d* BPatchSurf.inc/ImageConstants
* PURPOSE
*   These constants are for DrawPatch_UV###() macro and they define formats of UV_map bitmap file.
*   For details of UV mapping consult please POVRay documentation.
*
*   Following image formats are defined:    
* SYNOPSIS
*/
#declare _JPG =  1; // JPEG
#declare _BMP =  2; // BMP
#declare _GIF =  3; // GIF
#declare _PNG =  4; // PNG - default
#declare _TGA =  5; // Targa
#declare _SYS =  6; // depends on default set by operating system 
#declare _EXR =  7; // OpenEXR
#declare _HDR =  8; // High Dynamic Range format
#declare _IFF =  9; // IFF 
#declare _PGM = 10; // PGM
#declare _PPM = 11; // PPM
#declare _TIF = 12; // TIFF
/******/

/****iv* BPatchSurf.inc/DeclareLevel, tmpColour, tmpColour1, tmpColour2, DZero
* PURPOSE
*   DeclareLevel determines what is defined:
*   * 0 = colour only
*   * 2 = also texture
*   * 4 = complete material
*   this value is valid until next "Set...()" declaration is met
* SOURCE 
*/
#declare DeclareLevel = 0;                        // initial value
#declare tmpColour   = rgbf<0.2, 0.3, 0.4, 0.75>; // rgb <0, 0, 1>; well, I like this colour...
#declare tmpColour1  = rgbf<0.2, 0.3, 0.4, 0.75>; // ..transparent a little..
#declare tmpColour2  = rgbf<1.5, 1.3, 1.2, 0.73>; // additional, secondary colour used by some objects ex. in BarShapes.inc module
#declare DZero = 1E-4;    // minimal distance between grid & random point
#declare _AutoRange = 0;  // data range is set manually
/******/

#declare NumPts =   0;    // just initialization

/****f* BPatchSurf.inc/SetRGBFTColor
* PURPOSE
*   Sets default color stored in tmpColor internal variable:
*   Sets DeclareLevel to '0' (basic level)
* SYNOPSIS                      
*/
#macro SetRGBFTColor( RR, GG, BB, FF, TT )
/* 
* INPUTS
*   float RR - red component
*   float GG - green component
*   float BB - blue component
*   float FF - filter amount
*   float TT - transmit level
******/
    #declare tmpColour = rgbft<RR, GG, BB, FF, TT>;
    #declare DeclareLevel = 0;
#end

/****f* BPatchSurf.inc/SetTexture
* PURPOSE
*   declares texure for the enabled items and sets DeclareLevel on '2'
* SYNOPSIS                       
*/
#macro SetTexture( FullTexture ) // follow it by SetInterior("InteriorDefinition")
/*
* INPUTS
*   texturedef FullTexture - variable declared as fully defined valid texture
* EXAMPLE
#declare MyTexture = texture{pigment{color rgb <0,0,1>}};
SetTexture(MyTexture)
*****/
    #declare tmpTexture = FullTexture ;
    #declare DeclareLevel = 2;
#end

/****f* BPatchSurf.inc/SetInterior()
* PURPOSE
*   declares interior section for enabled items and sets DeclareLevel on '2'
* SYNOPSIS
*/ 
#macro SetInterior( tInt )       // valid only together with texture defintions
/* 
* INPUTS
*   interiordef tInt - variable declared as fully defined valid texture
* EXAMPLE
#declare TInterior = interior { // for details of 'interior' definition consult POVRay documentation
                        ior 1.5
                        fade_power 1001
                        fade_distance 0.9
                        fade_color <Nodes[CountY][CountX]/DscMaxY,.5,1-Nodes[CountY][CountX]/DscMaxY>
                      };
SetInterior(TInterior)
******/
    #declare tmpInterior = tInt;
    #declare DeclareLevel = 2;
#end

/****v* BPatchSurf.inc/DZero
* PURPOSE
*  minimal float number assumed not being equal zero
*  meaning of this variable depends on algorithm employed
* SEE ALSO
*  one should also refer to:
*   SetDZero (float)
* SOURCE
*/
#declare DZero = 1E-4;
/*******/

/****f* BPatchSurf.inc/SetDZero
* PURPOSE
*  minimal minimal float number assumed not being equal zero
*  meaning of this variable depends on algorithm employed
* SYNOPSIS
*/
#macro SetDZero(_dz)
/* 
* INPUTS
*  float _dz - new DZero value
* SEE ALSO
*  one should also refer to:
*   DZero
*******/
   #declare DZero = _dz;
#end

/****v* BPatchSurf.inc/VScale
* PURPOSE
*  float - Surface enhancement along vertical axis (negative values allowed ;)
*  this value is valid until "SetVScale()"
* SEE ALSO
*  one should also refer to:
*   SetVScale ()
* SOURCE
*/
#declare VScale = 1.0;    // Surface enhancement along vertical axis (negative values allowed ;)
/*******/

/****f* BPatchSurf.inc/SetVScale
* PURPOSE
*  Sets factor for enhancement along vertical axis (negative values allowed ;)
* SYNOPSIS                      
*/
#macro SetVScale( _ss)
/* 
* INPUTS
*  float _ss - new VScale value
* SEE ALSO
*  one should also refer to:
*   VScale
*******/
    #declare VScale = _ss; // Surface enhancement along vertical axis (negative values allowed ;)
#end

/****v* BPatchSurf.inc/VShift
* PURPOSE
*  float - see SetVShift macro
* SEE ALSO
*  one should also refer to:
*   SetVShift
* SOURCE
*/
#declare VShift = 0; // default value
/*******/

/****f* BPatchSurf.inc/SetVShift()
* PURPOSE
*  macro and variable, explanation:
*  due to limitations of bicubic interpolation algorithm, unexpected micro bulges are sometimes observed.
*  This can cause inconsistences of gradient mapping.
*  To overcome this difficulty one can shift whole surface up or down by applying this macro.
*  Values in range of 10^-4 are usually sufficient
* SYNOPSIS
*/
#macro SetVShift( _vs)
/* 
* INPUTS
*  float _vs - new VShift value
* SEE ALSO
*  one may also refer to:
*   VShift
*****/
   #declare VShift = _vs;
#end

/****f* BPatchSurf.inc/Set_AutoRange
* PURPOSE
*  data range will be computed
* SYNOPSIS
*/
#macro Set_AutoRange()
/* 
* INPUTS
*  none
* SEE ALSO
*  one should also refer to:
*   Cancel_AutoRange
*******/
    #declare _AutoRange = 1;
#end

/****f* BPatchSurf.inc/Cancel_AutoRange
* PURPOSE
*  data range will be set manually
* SYNOPSIS
*/
#macro Cancel_AutoRange()
/* 
* INPUTS
*  none
* SEE ALSO
*  one should also refer to:
*   Set_AutoRange
*******/
    #declare _AutoRange = 0;
#end

/****f* BPatchSurf.inc/Inline functions
* PURPOSE
*  helper functions applicable to macro: FunctionSurface(Func, x1, x2, Nxx, y1, y2, Nyy)
*  used to draw surface by equation z=Func( x, y)
* SYNOPSIS
#declare fId = function(i, j){ ..single nonconditional statement.. }; see example
* INPUTS
*  i, j - two float numbers for x, y argument values respectively, note that 'x' and 'y' symbols are predefined in POVRay as coord's system vectors
* EXAMPLE
#declare SinCos3 = function(i, j){ 2*sin(pi/2*i)+cos(pi/3*j)+i*j/400 }
FunctionSurface(SinCos3, 0, 10, 11, 0, 10, 11)
DrawAllPatches()
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), DrawPatch(), DrawAllPatchesGeo(), DrawAllPatches(), 
*   DrawAllPatchesWithTexture(), DrawAllPatchesWithMap(), POVRay function pointers
* SOURCE 
*/
#declare Cos2Sin = function(i, j){ 2*cos(1*(i*pi/18))*cos(1*(i*pi/18))-sin(2*(j*pi/18))}
#declare SinCosP2= function(i, j){ 2*sin(j*pi/18)*pow(  cos(i*pi/18),2) }
#declare SinCos3 = function(i, j){ 2*sin(pi/2*i)+cos(pi/3*j)+i*j/400 }
/******/

/****f* BPatchSurf.inc/Inline kriging functions()
* PURPOSE
*  helper functions applicable to macro:
*  smoothing & kriging functions: f=Func(float d)
* SYNOPSIS
#declare fId = function(i, j){ ..single nonconditional statement.. }; // see example below
* INPUTS
*  i, j - two float numbers for x,y argument values respectively, note that 'x' and 'y' symbols are predefined in POVRay as coord's system vectors
* EXAMPLE
#declare Newton  = function (d) { 1/d      }      // named after potential function
RawDataSurface("AnyFile.dat", Newton, Llimiter, .5, 0, 10, 20, 0, 10, 20)
DrawAllPatches()
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), DrawPatch(), DrawAllPatchesGeo(), DrawAllPatches(), 
*   DrawAllPatchesWithTexture(), DrawAllPatchesWithMap(), POVRay function pointers
* SOURCE
*/ 
#declare Newton   = function (d) { 1/d     }         // USELESS named after newtonian potential function
#declare Limiter  = function (d) { 1/DZero }        // this function is called when d<DZero

#declare Newton2  = function (d) { 1/pow(d,2)      } // named after newtonian potential function
#declare Limiter2 = function (d) { 1/pow(DZero,4)  }        // this function is called when d<DZero

#declare Newton4  = function (d) { 1/pow(d,2)      } // named after newtonian potential function
#declare Limiter4 = function (d) { 1/pow(DZero,4)  }        // this function is called when d<DZero
#declare Exponent = function (d) { exp(-d)  }        //
#declare Gauss    = function (d) { exp(-pow(d,2))}   // resembles gauss PDF distribution function

// simple kriging - more general example, which needs  
// two input values, interpreted as 'distance' and 'power' respectively
#declare SKrig   = function (d, pw) { 1/(1.0+pow(d, pw)) }  // SKrig(distance, power)

// ======[ specialised function ]======
#declare SKrig1  = function (d) { 1/(1.0+d) }
#declare SKrig2  = function (d) { 1/(1.0+pow(d,2)) }
#declare SKrig4  = function (d) { 1/(1.0+pow(d,4)) }
/******/

/****f* BPatchSurf.inc/DrawCardPoints
* PURPOSE
*  this helper macro renders all "cardinal" ie. corner points of bicubic patches in the scene as spheres
*  it is possible due to proper data structure. Macro renders pure geomtry, one have to define texture
*  in #declare/object{} construct
* SYNOPSIS
*/
#macro DrawCardPoints( R)  // Plots cardinal points of bicubic patches only
/* 
* INPUTS
*  float R - radius of cardinal point
* EXAMPLE
#declare PPP = object{DrawCardPoints(.2)};
object{ PPP
        texture {valid texture definition}
}
* SEE ALSO
*  one should also refer to:
*   object{} description in POVRay documentation
******/
   #declare ipy=0;
   #while (ipy<Ny-1)
      #declare ipx=0;
      #while (ipx<Nx-1)
         sphere{
              <PtV[Qds[ipy][ipx][0][0]].x, PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, PtV[Qds[ipy][ipx][0][0]].z>, R
              texture{ pigment { color Red }}
         }
         #declare ipx=ipx+1;
      #end
      // next (horizontal) row
      #declare ipy=ipy+1;
   #end
#end  // macro


/****M* BPatchSurf.inc/DrawPatch
* PURPOSE
*  this helper macro renders single bezier_patch{}
* SYNOPSIS                    
*/
#macro DrawPatch(ipy, ipx)
/* 
* INPUTS
*  uinteger ipy, uinteger ipx - two unsigned integer numbers, indices to array which contains definition of all patches
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), DrawPatch_UV (), EvalPatchedSurface (), DrawAllPatchesGeo (), 
*   DrawAllPatchesWithTexture (), DrawAllPatchesWithMap (), source code of this module
******/
   bicubic_patch{
       type 1
       flatness 0.0
       u_steps 4 v_steps 4
       PtV[Qds[ipy][ipx][0][0]], PtV[Qds[ipy][ipx][0][1]], PtV[Qds[ipy][ipx][0][2]], PtV[Qds[ipy][ipx][0][3]],
       PtV[Qds[ipy][ipx][1][0]], PtV[Qds[ipy][ipx][1][1]], PtV[Qds[ipy][ipx][1][2]], PtV[Qds[ipy][ipx][1][3]],
       PtV[Qds[ipy][ipx][2][0]], PtV[Qds[ipy][ipx][2][1]], PtV[Qds[ipy][ipx][2][2]], PtV[Qds[ipy][ipx][2][3]],
       PtV[Qds[ipy][ipx][3][0]], PtV[Qds[ipy][ipx][3][1]], PtV[Qds[ipy][ipx][3][2]], PtV[Qds[ipy][ipx][3][3]]
   }
#end

/****M* BPatchSurf.inc/DrawAllPatches
* PURPOSE
*  this helper macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure.
*  This macro applies default texture to the surface, so one don't need another
* SYNOPSIS
*/
#macro DrawAllPatches()  // default pigment gradient
/* 
* INPUTS
*  None
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), EvalPatchedSurface (), DrawPatch (), DrawAllPatchesGeo (), 
*   DrawAllPatchesWithTexture (), DrawAllPatchesWithMap ()
******/
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch(iy,ix)
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   texture {pigment {    // create a smooth color gradation map
      gradient y
      color_map {Cartographic_Colors}  // default choice
      }
      finish { diffuse 0.75 phong 1 ambient .3}
      scale <1, 1.05*(SceneMaxY-SceneMinY), 1>
      translate<0,-0.025,0> // arbitrary translation, must parametrize later
   }
   translate <0, SceneMinY, 0>
}
#end

/// cleaned down
/****M* BPatchSurf.inc/DrawAllPatchesWithTexture
* PURPOSE
*  this helper macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure.
*  This macro applies default texture to the surface, so one don't need another
* SYNOPSIS
*/
#macro DrawAllPatchesWithTexture( _MyTexture)  // default pigment gradient
/* 
* INPUTS
*  texturedef _MyTexture - any valid texture definition
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), EvalPatchedSurface (), DrawPatch (), DrawAllPatches (), 
*   DrawAllPatchesGeo (), DrawAllPatchesWithMap ()
******/
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch(iy,ix)
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   texture { _MyTexture
      scale <1, 1.05*(SceneMaxY-SceneMinY), 1>
      translate<0,-0.025,0> // arbitrary translation, must parametrize later
   }
   translate <0, SceneMinY, 0>
}
#end

/****M* BPatchSurf.inc/DrawAllPatchesWithMap
* PURPOSE
*  this macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure.
*  This macro applies default texture map to the surface, so one don't need another
* SYNOPSIS
*/
#macro DrawAllPatchesWithMap( _ThisMap)
/* 
* INPUTS
*  colormapdef _ThisMap - any valid color_map{} definition, uses passed color_map{} object,
*                         one from ColorMaps.inc or another self defined
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), EvalPatchedSurface (), DrawPatch (), DrawAllPatches (), 
*   DrawAllPatchesGeo (), DrawAllPatchesWithTexture ()
******/
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch(iy,ix)
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   texture {
      pigment {
             gradient y
             color_map {_ThisMap}
      }
      finish { diffuse 0.75 phong 1 ambient .3}
      scale <1, 1.05*(SceneMaxY-SceneMinY), 1>
      translate<0,-0.025,0> // arbitrary translation, must parametrize later
   }
   translate <0, SceneMinY, 0>
}
#end                                                 

/****M* BPatchSurf.inc/DrawPatch_UVpng
* PURPOSE
*  This macro renders single bezier_patch{} depicted by given indices.
*  Its simplicity results from sophisticated data structure.
*  This macro applies UV'ed texture to the surface given in _UV_png (PNG format) file.
* SYNOPSIS
*/
#macro DrawPatch_UVpng(ipy, ipx, _UV_png)
/* 
* INPUTS
*  integer ipy - 
*  integer ipx - indices to given bezier patch item 
*  string_fn _UV_png - filename of PNG raster bitmap
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), EvalPatchedSurface (), DrawPatch (), DrawAllPatches_UVpng (),
*   DrawAllPatches (), DrawAllPatchesGeo (), DrawAllPatchesWithTexture ()
******/
   bicubic_patch{                             
       type 1             
       flatness 0.0                                         
       u_steps 4 v_steps 4
       PtV[Qds[ipy][ipx][0][0]], PtV[Qds[ipy][ipx][0][1]], PtV[Qds[ipy][ipx][0][2]], PtV[Qds[ipy][ipx][0][3]],           
       PtV[Qds[ipy][ipx][1][0]], PtV[Qds[ipy][ipx][1][1]], PtV[Qds[ipy][ipx][1][2]], PtV[Qds[ipy][ipx][1][3]],
       PtV[Qds[ipy][ipx][2][0]], PtV[Qds[ipy][ipx][2][1]], PtV[Qds[ipy][ipx][2][2]], PtV[Qds[ipy][ipx][2][3]],
       PtV[Qds[ipy][ipx][3][0]], PtV[Qds[ipy][ipx][3][1]], PtV[Qds[ipy][ipx][3][2]], PtV[Qds[ipy][ipx][3][3]]

       texture { 
          uv_mapping 
          pigment { 
               image_map { png _UV_png  map_type 0   interpolate 2}
          }            
//          finish { diffuse 0.75 phong .5 ambient .63}  // maybe? :P
       }  
    }
#end
//============================


/****M* BPatchSurf.inc/DrawAllPatches_UVpng()
* PURPOSE
*  This macro renders single bezier_patch{} depicted by given indices.
*  Its simplicity results from sophisticated data structure.
*  This macro applies UV'ed texture to the surface given in _UV_png (PNG format) file.
* SYNOPSIS
DrawAllPatches_UVpng(_png_file )
* INPUTS
*  string_fn _UV_png - filename of (square) PNG raster bitmap
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UVpng(),
*   DrawAllPatches(), DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro DrawAllPatches_UVpng( _png_file)  // default pigment gradient
union{
   #declare iy=0;
   #while (iy<Ny-1)  
      #declare ix=0;                            
      #while (ix<Nx-1)
         DrawPatch_UVpng(iy, ix, _png_file )
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end  

   translate <0, SceneMinY, 0>                 
   double_illuminate
}             
#end                                          
//============================                                           


/****M* BPatchSurf.inc/DrawAllPatchesGeo
* PURPOSE
*  this macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure.
*  This macro applies NO texture to the surface, so one MUST aplly it by hand
* SYNOPSIS                              
*/
#macro DrawAllPatchesGeo() //geometry only !!!
/*
* INPUTS
*  None
* SEE ALSO
*  one should also check:
*   FunctionSurface (), EvalPatchedSurface (), DrawPatch (), DrawAllPatches (), 
*   DrawAllPatchesWithTexture (), DrawPatch_UVpng
******/
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch(iy,ix)
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   translate <0, SceneMinY, 0>
}
#end

///=============[ introduced 2012-11-26 ]===========
///===============================================
/// _type    - format of image passed to macro
/// _type =[ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
/// _UV_png  - filename of single cell pattern
///
/****M* BPatchSurf.inc/DrawPatch_UV()
* PURPOSE
*  this macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure.
*  This macro applies NO texture to the surface, so one MUST aplly it by hand
* SYNOPSIS
DrawPatch_UV(uinteger ipy, uinteger ipx, ImageConstants _type, string filename _UV_png)
* INPUTS
*  uinteger ipy   - and  ..
*  uinteger ipx   - two unsigned integer numbers, indices to array which contains definition of all patches
*  ImageConstants _type    - indicates format of the bitmap passed to the macro as uv_map{}
*  _type = [ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
*  string filename _UV_png - valid filename of bitmap of type declared above
*  this bitmap should be perpendicular, preferably rectangular, some images are distributed with the package
*  as G00.jpg to G10.jpg as well as Gridd*.jpg files in "./img" subfolder
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), DrawPatch(), EvalPatchedSurface(), DrawPatch(), DrawAllPatches(), 
*   DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro DrawPatch_UV(ipy, ipx, _type, _UV_png)
   bicubic_patch{
       type 1
       flatness 0.0
       u_steps 4 v_steps 4
       PtV[Qds[ipy][ipx][0][0]], PtV[Qds[ipy][ipx][0][1]], PtV[Qds[ipy][ipx][0][2]], PtV[Qds[ipy][ipx][0][3]],
       PtV[Qds[ipy][ipx][1][0]], PtV[Qds[ipy][ipx][1][1]], PtV[Qds[ipy][ipx][1][2]], PtV[Qds[ipy][ipx][1][3]],
       PtV[Qds[ipy][ipx][2][0]], PtV[Qds[ipy][ipx][2][1]], PtV[Qds[ipy][ipx][2][2]], PtV[Qds[ipy][ipx][2][3]],
       PtV[Qds[ipy][ipx][3][0]], PtV[Qds[ipy][ipx][3][1]], PtV[Qds[ipy][ipx][3][2]], PtV[Qds[ipy][ipx][3][3]]

       texture {
          uv_mapping
            #switch ( _type )
                #case (_JPG) pigment { image_map{ jpeg _UV_png map_type 0 interpolate 2}}  #break
                #case (_BMP) pigment { image_map{ bmp  _UV_png map_type 0 interpolate 2}}  #break
                #case (_GIF) pigment { image_map{ gif  _UV_png map_type 0 interpolate 2}}  #break
                #case (_PNG) pigment { image_map{ png  _UV_png map_type 0 interpolate 2}}  #break
                #case (_TGA) pigment { image_map{ tga  _UV_png map_type 0 interpolate 2}}  #break
                #case (_SYS) pigment { image_map{ sys  _UV_png map_type 0 interpolate 2}}  #break
                #case (_EXR) pigment { image_map{ exr  _UV_png map_type 0 interpolate 2}}  #break
                #case (_HDR) pigment { image_map{ hdr  _UV_png map_type 0 interpolate 2}}  #break
                #case (_IFF) pigment { image_map{ iff  _UV_png map_type 0 interpolate 2}}  #break
                #case (_PGM) pigment { image_map{ pgm  _UV_png map_type 0 interpolate 2}}  #break
                #case (_PPM) pigment { image_map{ ppm  _UV_png map_type 0 interpolate 2}}  #break
                #case (_TIF) pigment { image_map{ tiff _UV_png map_type 0 interpolate 2}}  #break
                #else pigment {  color rgbf <.5,.5,.9,.8> } #break
            #end
            finish { diffuse 0.75 phong .5 ambient .63}
       }
    }
#end

///===============================================
/// _type    - format of image passed to macro
/// _type =[ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
/// _UV_png  - filename of single cell pattern
/// _max parameter of mod(N, _max) operation
/// chooses from a set of images named after the pattern: _UV_core##.ext
///
/****M* BPatchSurf.inc/DrawPatch_UV_multi()
* PURPOSE
*  this macro renders single bezier_patch{} and applies UV texture to it
* SYNOPSIS
*/
#macro DrawPatch_UV_multi(ipy, ipx, _type, _UV_core, _max)
/*
* INPUTS
*  uinteger ipy   - and  ..
*  uinteger ipx   - two unsigned integer numbers, indices to array which contains definition of all patches
*  ImageConstants _type    - indicates format of the bitmap passed to the macro as uv_map{}
*  _type = [ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
*  string   _UV_core - constant part of image file
*  uinteger _max     - automatically generated suffix goes from '0' up to '_max-1'. Suffix is then concatenated
*                    - with _UV_core and extension defined by _type. Thus final image name is defined.
*                    - This image will be used as uv_map for the given by ipx&ipy indices patch.
*                    - Consult source file for details.
* EXAMPLE
DrawPatch_UV_multi(3, 5, _JPG, "Next", 7)
// renders patch 3rd/5th with uv image named: "Next01.jpg"
* SEE ALSO
*  one should also refer to:
*   FunctionSurface (), DrawPatch (), DrawPatch_UV (), EvalPatchedSurface (), DrawPatch (), 
*   DrawAllPatches (), DrawAllPatchesGeo (), DrawAllPatchesWithTexture ()
******/
   bicubic_patch{
       type 1
       flatness 0.0
       u_steps 4 v_steps 4
       PtV[Qds[ipy][ipx][0][0]], PtV[Qds[ipy][ipx][0][1]], PtV[Qds[ipy][ipx][0][2]], PtV[Qds[ipy][ipx][0][3]],
       PtV[Qds[ipy][ipx][1][0]], PtV[Qds[ipy][ipx][1][1]], PtV[Qds[ipy][ipx][1][2]], PtV[Qds[ipy][ipx][1][3]],
       PtV[Qds[ipy][ipx][2][0]], PtV[Qds[ipy][ipx][2][1]], PtV[Qds[ipy][ipx][2][2]], PtV[Qds[ipy][ipx][2][3]],
       PtV[Qds[ipy][ipx][3][0]], PtV[Qds[ipy][ipx][3][1]], PtV[Qds[ipy][ipx][3][2]], PtV[Qds[ipy][ipx][3][3]]

       #local _numm=mod(ipx+ipy, _max);
       #local _suff=str(_numm, -2, 0);
       #local _UV_png = concat(_UV_core,_suff);
       texture {
          uv_mapping
            #switch ( _type )
                #case (_JPG) pigment { image_map{ jpeg concat(_UV_png,".jpg") map_type 0 interpolate 2}}  #break
                #case (_BMP) pigment { image_map{ bmp  concat(_UV_png,".bmp") map_type 0 interpolate 2}}  #break
                #case (_GIF) pigment { image_map{ gif  concat(_UV_png,".gif") map_type 0 interpolate 2}}  #break
                #case (_PNG) pigment { image_map{ png  concat(_UV_png,".png") map_type 0 interpolate 2}}  #break
                #case (_TGA) pigment { image_map{ tga  concat(_UV_png,".tga") map_type 0 interpolate 2}}  #break
                #case (_SYS) pigment { image_map{ sys  concat(_UV_png,".sys") map_type 0 interpolate 2}}  #break
                #case (_EXR) pigment { image_map{ exr  concat(_UV_png,".exr") map_type 0 interpolate 2}}  #break
                #case (_HDR) pigment { image_map{ hdr  concat(_UV_png,".hdr") map_type 0 interpolate 2}}  #break
                #case (_IFF) pigment { image_map{ iff  concat(_UV_png,".iff") map_type 0 interpolate 2}}  #break
                #case (_PGM) pigment { image_map{ pgm  concat(_UV_png,".pgm") map_type 0 interpolate 2}}  #break
                #case (_PPM) pigment { image_map{ ppm  concat(_UV_png,".ppm") map_type 0 interpolate 2}}  #break
                #case (_TIF) pigment { image_map{ tiff concat(_UV_png,".tif") map_type 0 interpolate 2}}  #break
                #else pigment {  color rgbf <.5,.5,.9,.8> } #break
            #end
            finish { diffuse 0.75 phong .5 ambient .63}
       }
    }
#end

///===============================================
/// _type - format of image passed to macro
/// _type = [ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF ]
/// _file - filename of single cell pattern
/****M* BPatchSurf.inc/DrawAllPatches_UV()
* PURPOSE
*  This macro renders all bezier_patch{}'es in single surface
*  its simplicity results from sophisticated data structure :)
*  The macro applies UV texture to the whole surface.
* SYNOPSIS
DrawAllPatches_UV(ImageConstants _type, string filename _file)
* INPUTS
*  ImageConstants _type - indicates format of the bitmap passed to the macro as uv_mapping{}.
*  _type = [ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF ]
*  string filename - Name of map for the surface
*                  - Consult source file for details.
* EXAMPLE
DrawAllPatches_UV( _PNG, "GriddVividRed.png") // covers surface with copies of "GriddVividRed.png" file
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UV(), DrawPatch_UV_multi(),
*   DrawPatch(), DrawAllPatches(), DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro DrawAllPatches_UV( _type, _file)  // default pigment gradient
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch_UV(iy, ix, _type, _file )
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end

   translate <0, SceneMinY, 0>
   double_illuminate
}
#end
///===============================================

///===============================================
/// rotates images over patches - chooses from a set of images named after the pattern: _UV_core##.ext
/// DrawAllPatches_UV_multi(_type ,      // bitmap type - see below
///                         _fileprefix, // prefix of the file
///                         _max)       // divisor in modulo operation.
/// _type    - format of image passed to macro
/// _type =[ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
/// _file - filename of single cell pattern
/// _file has to obey rule core##.ext, (# - single digit '0' to '9')
///           counted from '00' up to _max-1 for ex. G00.jpg, barn07.tga
/// _ext = [ exr | gif | hdr | iff | jpg | pgm | png | ppm | sys | tga | tif]
/// _max = 2..100/ parameter of mod(N, _max) operation
///
/****M* BPatchSurf.inc/DrawAllPatches_UV_multi()
* PURPOSE
*  this macro renders all bezier_patch{}'es in this surface and
*  applies UV texture to a every single patch by iterative formula
* SYNOPSIS
DrawAllPatches_UV_multi(ImageConstants _type, string _file, uinteger _max)
* INPUTS
*  ImageConstants _type    - indicates format of the bitmap passed to the macro as uv_map{}
*  _type = [ _JPG | _BMP | _GIF | _PNG |_TGA | _SYS |_EXR |_HDR |_IFF |_PGM |_PPM |_TIF |_TIF ]
*  string   _file - constant part of image file name
*  uinteger _max     - automatically generated suffix goes from '0' up to '_max-1'. Suffix is then concatenated
*                    - with _file and extension defined by _type. Thus final image name is defined.
*                    - This image will be used as uv_map for consecutive patches. Calls macro DrawPatch_UV_multi().
*                    - _max = 2..100/ parameter of mod(N, _max) operation. Consult source file for details.
* EXAMPLE
DrawAllPatches_UV_multi(_JPG, "Next", 7)
// renders all patches and uses files Next00.jpg through Next06.jpg as maps for consecutive patches
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UV(), DrawPatch(),
*   DrawAllPatches(), DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro DrawAllPatches_UV_multi( _type, _file, _max)  // default pigment gradient
union{
   #declare iy=0;
   #while (iy<Ny-1)
      #declare ix=0;
      #while (ix<Nx-1)
         DrawPatch_UV_multi(iy, ix, _type, _file, _max )
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end

   translate <0, SceneMinY, 0>
   double_illuminate
}
#end
///===============================================
///===============================================
///===============================================
///================[ 2012-11-26 ]=================

/****M* BPatchSurf.inc/MatrixFileSurface()  [main macro]
* PURPOSE
*  Imports data from file InFileName, prepares data structure for EvalPatchedSurf() macro and eventually calls it.
*    Format of input file (like matrix - Cx is for Column, Ry for Row):
*    +------> [ rows mapped along X real] >----->     +------> [ X real] >----->
*    |  C1R1, C2R1, C3R3, ...........                 |  z(x1y1), z(x2y1), z(x3y3), .....
*    |  C1R2,                                         |  z(x1y2), z(x2y2),
*    |  C1R3,                                         |  z(x1y3)),
*    v   . .                                          v   . .
*    [column mapped along Y real]                     [Y real]
*  Macro is provided for drawing data in real coordinates. Data in a row have the same y coordinate
*  but sequencial x's. Only 'z' (height) values are given, values for x&y are calculated from values given in the header
*  of the input file. Watching at the data file you see resulting surface like seeing it vertically from bottom of
*  the coordinate system, having X axis oriented toward right hand, and Y axis beforehand.
* SYNOPSIS
MatrixFileSurface(string_fn InFileName)
* INPUTS
*  string_fn filename InFileName - valid filename eg. "InSurfData.dat". Extension is not required nor fixed.
* EXAMPLE
MatrixFileSurface("InSurfData.dat") // import data..
DrawAllPatches()                    // and then render all patches
* SEE ALSO
*  one should also refer to:
*   FunctionSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UV(), DrawPatch(),
*   DrawAllPatches(), DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro MatrixFileSurface(InFileName)
#debug concat(InFileName, "   \n")
#ifdef (DataFile)
    #undef DataFile
#end
  #fopen DataFile InFileName read
    #read(DataFile, DescriptX, DescriptY)
    #read(DataFile, Nx, x1, x2)
    #read(DataFile, Ny, y1, y2)
    #if ((Nx>1)&(Ny>1)) // at least 2x2 mesh

      #declare Nodes=array[Ny][Nx];    // float: height of nodes only, x, y defined directly
      #declare Nodes[0][0]=0.0;        // define as float!

      #declare MaxNx=3*(Nx-1)+1;       // number of all nodes (cardinal+control) in 'x' direction
      #declare MaxNy=3*(Ny-1)+1;       // number of all nodes (cardinal+control) in 'y' direction

      #declare AllVertices = MaxNx*MaxNy;
      #declare PtV = array[AllVertices];       // vectors, real coords of all points
      #declare Pts = array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
      #declare Qds = array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points

// Initialization of all points
      #declare iy=0;
      #while(iy<Ny)
        #declare ix=0;
        #while(ix<Nx)
          #read(DataFile, Nodes[iy][ix])
          #declare ix=ix+1;
        #end
        #declare iy=iy+1;
      #end


      #local deltax = (x2-x1)/(MaxNx-1);
      #local deltay = (y2-y1)/(MaxNy-1);

      #declare iy = 0;
      #while (iy < MaxNy)
         #declare ix = 0;
         #while (ix < MaxNx) // !! left handed Pov Coord system:  y=xpov, x=-zpov, z=y !! (2012-11-12)
            #local zpov = -ix*deltax-x1; // x //!! changed 2012-05-10
            #local xpov = iy*deltay+y1;  // y //!! changed 2012-05-10

//!! changed 2012-05-10
            #declare PtV[MaxNx*iy+ix] = <xpov, 0, zpov>;  // 'Pointed Vectors by': 'y' for cardinal points define later on
            #declare Pts[iy][ix] = MaxNx*iy+ix;           // 'Pointers to': indices to the vector above
            #declare ix = ix+1;
         #end
         #declare iy = iy+1;
      #end

      #declare SceneMaxX = 0;  #declare SceneMinX = 0;
      #declare SceneMaxY = 0;  #declare SceneMinY = 0;
      #declare SceneMaxZ = 0;  #declare SceneMinZ = 0;

      #declare iy=0;
      #while (iy<Ny)
         #declare ix=0;
         #while (ix<Nx) // !! left handed Pov Coord system: y=xpov, x=-zpov, z=y !! (2012-11-12)
// now define main storage table, cardinal points only
            #local PtVCurr=PtV[3*(MaxNx*iy+ix)];         // 'Current Vector', temporary variable
            #local xpov = PtVCurr.x;                     // y real
            #local zpov = PtVCurr.z;                     // -x real
            #local ypov = VScale*Nodes[iy][ix]+VShift;   // 'z' in real world
            #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, zpov>;

// evaluate bounding cube in PovRay coords..
            #local PtVCurr=PtV[3*(MaxNx*iy+ix)];
            #if (PtVCurr.x > SceneMaxX)
               #declare SceneMaxX=PtVCurr.x;
            #end
            #if (PtVCurr.x < SceneMinX)
               #declare SceneMinX=PtVCurr.x;
            #end

            #if (PtVCurr.y > SceneMaxY)
               #declare SceneMaxY=PtVCurr.y;
            #end
            #if (PtVCurr.y < SceneMinY)
               #declare SceneMinY=PtVCurr.y;
            #end

// left-handed coordinate system: Z-axis is reversed!!
            #if (PtVCurr.z < SceneMaxZ)
               #declare SceneMaxZ=PtVCurr.z;
            #end
            #if (PtVCurr.z > SceneMinZ)
               #declare SceneMinZ=PtVCurr.z;
            #end

            #declare ix=ix+1;
         #end
         #declare iy=iy+1;
      #end
      #declare ZCoeff=(SceneMaxY>SceneMinY?1.0/(SceneMaxY-SceneMinY):1.0); // to avoid division by zero for flat surfaces..
// finally create all patches:
     EvalPatchedSurface()
// ready!
    #else
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
      #debug "* To few nodes. You need at least 2x2=4 nodes!  *\n"
      #debug "* * * * * * * * * * * * * * * * * * * * * * * * *\n"
    #end  // main if#if ((Nx>1)&(Ny>1)) // at least 2x2 mesh

    #fclose DataFile  // 2012-11-10
#end


/****M* BPatchSurf.inc/FunctionSurface, EvalFuncPatchedSurf  [done]
* PURPOSE
*  Draws surface defined by z=Func(x,y) function, defined in real world parameters
* SYNOPSIS
FunctionSurface(pfunction Func, float x1, float x2, integer Nxx, float y1, float y2, integer Nyy )
* INPUTS
*  pfunction Func -> inline f(x,y) => #declare abc=function(x,y){x-y} function declared in real x,y,z coordinates
*  float x1, float x2, integer Nxx => x-min, x-max, num. of cardinal (exact value) points
*  float y1, float y2, integer Nyy => the same but for y axis
* EXAMPLE
#declare SinCos3 = function(i, j){ 2*sin(pi/2*i)+cos(pi/3*j)+i*j/400 }
FunctionSurface(SinCos3, 0, 4*pi, 20, 0, 4*pi, 20) // prepare SinCos3(x,y) surface over rectangle
                                                   // extending from 0 to 4*Pi on X and Y axis
DrawAllPatches()                    // and then render it
* SEE ALSO
*  one should also refer to:
*   MatrixFileSurface (), EvalPatchedSurface (), DrawPatch (), DrawPatch_UV (), DrawPatch (),
*   DrawAllPatches (), DrawAllPatchesGeo(), DrawAllPatchesWithTexture (), EvalFuncPatchedSurf ()
******/
#macro FunctionSurface(Func, x1, x2, Nxx, y1, y2, Nyy)

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
   // Initialization of all points
   #declare iy=0;
   #while (iy<MaxNy)
      #declare ix=0;
      #while (ix<MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
         #declare Pts[iy][ix]= MaxNx*iy+ix;               // indices to the vector above
         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end

   #declare SceneMaxX=0;
   #declare SceneMinX=0;

   #declare SceneMaxY=0;
   #declare SceneMinY=0;

   #declare SceneMaxZ=0;
   #declare SceneMinZ=0;

   #local deltax=(x2-x1)/(MaxNx-1);
   #local deltay=(y2-y1)/(MaxNy-1);

   #declare iy=0;
   #while (iy<MaxNy)
      #declare ix=0;
      #while (ix<MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!

//!! changed 2012-05-10
         #local xreal = ix*deltax+x1; // x
         #local yreal = iy*deltay+y1; // y
         #local ypov = VScale*Func(xreal, yreal)+VShift;     // z

#ifdef(Debug)
  #if(Debug>0)  
    #debug concat("x= ",str(xreal, 7, 5), "y= ",str(yreal, 7, 5), "y= ", str(ypov, 10, 8), "\n")         
  #end  
#end    
///         #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points - original
         #declare PtV[MaxNx*iy+ix] = <yreal, ypov, -xreal>;  // real vectorsw/points in PovRay coord system : experimental in real world-[0Y,0Z,0X]
         #local PtVCurr=PtV[MaxNx*iy+ix];

         #if (PtVCurr.x > SceneMaxX)
            #declare SceneMaxX=PtVCurr.x;
         #end
         #if (PtVCurr.x < SceneMinX)
            #declare SceneMinX=PtVCurr.x;
         #end

         #if (PtVCurr.y > SceneMaxY)
            #declare SceneMaxY=PtVCurr.y;
         #end
         #if (PtVCurr.y < SceneMinY)
            #declare SceneMinY=PtVCurr.y;
         #end
         // left-handed coordinate system: Z-axis is reversed!!
         #if (PtVCurr.z < SceneMaxZ)
            #declare SceneMaxZ=PtVCurr.z;
         #end
         #if (PtVCurr.z > SceneMinZ)
            #declare SceneMinZ=PtVCurr.z;
         #end

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end

#macro EvalFuncPatchedSurf(Func) // Evaluates data for func(x,y) patched bicubic surface - default settings - obsolete
  FunctionSurface(Func, 0, 10, 20, 0, 10, 20) // create PtV[], Pts[], Qds[] i.e. all surface patches: Func, xmin, xmax, Nx; ymin, ymax, Ny
#end


/****M* BPatchSurf.inc/RawDataSurfaceExt(), RawDataSurface()  [to be done]
* PURPOSE
*  RawDataSurfaceExt() is just wrapper for RawDataSurface() function
* SYNOPSIS
* INPUTS
* EXAMPLE
RawDataSurfaceExt("SSurfA.dat", Gauss, Gauss, 0, 10, 20, 0, 10, 20) //
DrawAllPatches()                    // and then render it

OR

RawDataSurface("SSurfA.dat", Gauss, Gauss, 2.0,  0, 10, 20, 0, 10, 20) //
DrawAllPatches()                    // and then render it
* SEE ALSO
*  one should also refer to:
*   RawDataSurface(), MatrixFileSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UV(), DrawPatch(),
*   ScatterDataSurfaceExp(), DrawAllPatches(), 
*   DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/

/// comment marked as ready to delete
// The most configurable version
// Potential -> f(x,y) => smoothing potential, 1/r, 1/r^2, .., exp(-a*r), ...
// Input file:
// "Comment X", "Comment Y"
// num_of_data
// x_1, y_1, z_1
// x_2, y_2, z_2
// .............
// x1, x2, Nxx    => x-min, x-max, num. of cardinal (exact value) points
// y1, y2, Nyy    => the same but for y axis
// Smooth   => smoothing potential function
// Singular => what to do if grid point is too close to data dile point
// ex: with potential funcion V(r1, r0):=1/|r1-r0| there singularity at point
//     r1=r0. So we need special function for solving this problem.
// Wrapper for backward compatibility
/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#macro RawDataSurfaceExt(InFileName, Smooth, Singular, x1, x2, Nxx, y1, y2, Nyy)
    RawDataSurface(InFileName, Smooth, Singular, 1.0, x1, x2, Nxx, y1, y2, Nyy)
#end

#macro RawDataSurface(InFileName, Smooth, Singular, sigma, x1, x2, Nxx, y1, y2, Nyy)

// Now open data file and import all points
   #fopen DataFile InFileName read
   #read(DataFile, DescriptX, DescriptY, DescriptZ)
   #read(DataFile, NumPts)
   #declare RNodes = array[NumPts];    // float vector: height of nodes AND x, y
   #declare RNodes[0] = <0.0, 0.0, 0.0>;
   #declare rix=0;

   #while(rix < NumPts)
      #read(DataFile, xx, yy, zz)
      #declare RNodes[rix] = <xx, yy, VScale*zz+VShift>; // real world
      #declare rix = rix + 1;        
   #end
   #fclose DataFile
//==========================//

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
// Initialization of ALL points
/// comment marked as ready to delete
//   #declare iy=0;
//   #while (iy<MaxNy)
//      #declare ix=0;
//      #while (ix<MaxNx)                   // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
//         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
//         #declare Pts[iy][ix]= MaxNx*iy+ix;     // indices to the vector above
//         #declare ix=ix+1;
//      #end
//      #declare iy=iy+1;
//   #end
/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Now initialize arrays and define Nodes array
   #local deltax = (x2-x1)/(MaxNx-1);
   #local deltay = (y2-y1)/(MaxNy-1);

   #declare iy = 0;
   #while (iy < MaxNy)
      #declare ix = 0;
      #while (ix < MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local xpov = ix*deltax+x1; // x
         #local zpov = iy*deltay+y1; // y

         #declare PtV[MaxNx*iy+ix] = <zpov, 0, -xpov>;   // here corrected: <xpov, 0, -zpov>;  // 'y' for cardinal points define later on
         #declare Pts[iy][ix] = MaxNx*iy+ix;            // indices to the vector above
         #declare ix = ix+1;
      #end
      #declare iy = iy+1;
   #end

   #if (_AutoRange=0)  
   // I think it should be changed x<>y due to POvRay crazy Coord System
       #declare SceneMaxX = y2;   #declare SceneMinX = y1;
       #declare SceneMaxY =0.0;   #declare SceneMinY =0.0;
       #declare SceneMaxZ =-x2;   #declare SceneMinZ =-x1;   // little bit strange MaxZ < MinZ!!
    #else
       // search for min/max
       #local _xmi = RNodes[0].x;    #local _xma = RNodes[0].x;    
       #local _ymi = RNodes[0].y;    #local _yma = RNodes[0].y;
       #local _zmi = RNodes[0].z;    #local _zma = RNodes[0].z;
       #for(_ip,0, NumPts-1)
           #if (RNodes[_ip].x < _xmi) #local _xmi=RNodes[_ip].x; #end       
           #if (RNodes[_ip].x > _xma) #local _xma=RNodes[_ip].x; #end       
    
           #if (RNodes[_ip].y < _ymi) #local _ymi=RNodes[_ip].y; #end       
           #if (RNodes[_ip].y > _yma) #local _yma=RNodes[_ip].y; #end       
    
           #if (RNodes[_ip].z < _zmi) #local _zmi=RNodes[_ip].z; #end       
           #if (RNodes[_ip].z > _zma) #local _zma=RNodes[_ip].z; #end       
       #end   
       #if (Debug>0)   
         #debug "Raw points contained in the (real!) box:\n"
            #debug concat("MinX=",str(_xmi,6,4),"; MaxX=",str(_xma,6,4),"\n")
            #debug concat("MinY=",str(_ymi,6,4),"; MaxY=",str(_yma,6,4),"\n")
            #debug concat("MinZ=",str(_zmi,6,4),"; MaxZ=",str(_zma,6,4),"\n")    
         #debug concat("\'z\' scaling factor:", str(VScale, 10, 6),"\n")
         #debug "=================\n"        
       #end
       #declare SceneMaxX =  _yma;   #declare SceneMinX =  _ymi;
       #declare SceneMaxY =  _zma;   #declare SceneMinY =  _zmi;
       #declare SceneMaxZ = -_xma;   #declare SceneMinZ = -_xmi;   // little bit strange MaxZ < MinZ!!
       
   #end
//   #declare Nodes = array [Ny][Nx];
//   #declare Nodes[0][0] = 0.0;

// delta's for Qts matrix
   #declare iy=0;
   #declare wghtSum = 0.0;
   #declare wght = 0.0;
   #while (iy<Ny)
      #declare ix=0;
      #while (ix<Nx)              // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local PtVCurr = PtV[3*(MaxNx*iy+ix)];

         #local xpov =  PtVCurr.x; //   x
         #local zpov = -PtVCurr.z; // --y because I need real world values here
        
         #local ypov = 0.0;
         #local rix  = 0;
         #declare wghtSum = 0.0;
         #declare wght= 0.00;
         #while(rix<NumPts) // take into account all points from input file
//            #declare d  = sqrt(pow(xpov-RNodes[rix].x,2)+pow(zpov-RNodes[rix].y,2)); /// comment marked as ready to delete
            #declare d  = sqrt(pow(zpov-RNodes[rix].x,2)+pow(xpov-RNodes[rix].y,2))/sigma; // distance between approximated and reference points ///corrected
            #declare wght = (d>DZero?Smooth(d):Singular(d));
            #declare wghtSum = wghtSum+wght;
            #local ypov = ypov+RNodes[rix].z*wght;
            #local rix=rix+1;
         #end
 //        #local ypov = ypov/NumPts;  // normalization /// comment marked as ready to delete
         #declare wghtSum = (wghtSum>0?wghtSum:1.0);   // just for stability
//         #local ypov = VScale*ypov/wghtSum+VShift;
         #local ypov = ypov/wghtSum; // corrected 2016-10-06
         #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, -zpov>; // -zpov back to internal representation

/// comment marked as ready to delete
// #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points
//         #local PtVCurr = PtV[MaxNx*iy+ix];

//         #if (PtVCurr.x > SceneMaxX)
//            #declare SceneMaxX=PtVCurr.x;
//         #end
//         #if (PtVCurr.x < SceneMinX)
//            #declare SceneMinX=PtVCurr.x;
//         #end
/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

         #if (ypov > SceneMaxY)
            #declare SceneMaxY=ypov;
         #end
         #if (ypov < SceneMinY)
            #declare SceneMinY=ypov;
         #end
// left-handed coordinate system: Z-axis is reversed!!

/// comment marked as ready to delete
//         #if (PtVCurr.z < SceneMaxZ)
//            #declare SceneMaxZ=PtVCurr.z;
//         #end
//         #if (PtVCurr.z > SceneMinZ)
//            #declare SceneMinZ=PtVCurr.z;
//         #end
/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end

// compute smoothed value at point "_P"
#macro RawDataPoint(_P, Smooth, Singular, sigma)
         #local wghtSum = 0.0;
         #local wght = 0.00;
         #local ypov = 0.0;
         #local rix  = 0;
         #while(rix<NumPts) // take into account all points from input file
            #local d  = sqrt(pow(_P.x-RNodes[rix].x,2)+pow(_P.y-RNodes[rix].y,2))/sigma; // distance between approximated and reference points ///corrected
            #local wght = (d>DZero?Smooth(d):Singular(d));
            #local wghtSum = wghtSum+wght;
            #local ypov = ypov+RNodes[rix].z*wght;
            #local rix=rix+1;
         #end
         #declare wghtSum = (wghtSum>0?wghtSum:DZero);   // just for stability
         #local ypov = ypov/wghtSum; // corrected 2016-10-06 1 // 
         ypov
#end

#macro DrawDelta(Smooth, Singular, sigma, _rad, _fct) // sigma - smoothing range, _fct - scaling factor
#debug "Drawing delta \n"
    #ifdef(EstNodes)
       #local rix=0;      
       #declare _est = 0;
       #while(rix<NumPts)                              
           #local   _dst = EstNodes[rix][3] - RNodes[rix].z; // remembered vale                                
           #if (abs(_dst)>DZero)
    //       cylinder{<RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x> <RNodes[rix].y, _est, -RNodes[rix].x>, _rad}
             cylinder{<RNodes[rix].y, 0, -RNodes[rix].x> <RNodes[rix].y, _fct*_dst, -RNodes[rix].x>, _rad       
                  #if(_dst>0)
                     texture{pigment{color rgb <1, 0, 0>} finish{Dull emission .8}}
                  #else
                     texture{pigment{color rgb <0, 0, 1>} finish{Dull emission .8}}
                  #end  
                  no_shadow
             }    
           #else         
             sphere{<RNodes[rix].y, 0, -RNodes[rix].x>, _rad texture{pigment{color rgb <.0,1,0>} finish{Dull emission .8}} no_shadow}
           #end
           #local rix=rix+1;
       #end        
    #else
      #debug "You need to call ComputeDelta() function first. Do it now!\n"
    #end   
#end

#macro DrawDeltaBW(Smooth, Singular, sigma, _rad, _fct) // sigma - smoothing range, _fct - scaling factor
#debug "Drawing delta \n"
    #ifdef(EstNodes)
       #local rix=0;      
       #declare _est = 0;
       #while(rix<NumPts)                              
           #local   _dst = abs(EstNodes[rix][3] - RNodes[rix].z); // remembered vale                                
           #if (_dst>DZero)
    //       cylinder{<RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x> <RNodes[rix].y, _est, -RNodes[rix].x>, _rad}
             cylinder{<RNodes[rix].y, 0, -RNodes[rix].x> <RNodes[rix].y, _fct*_dst, -RNodes[rix].x>, _rad       
                  texture{pigment{color rgb 0 } finish{ Dull }}
                  no_shadow
             }    
           #else         
             sphere{<RNodes[rix].y, 0, -RNodes[rix].x>, _rad texture{pigment{color rgb .5 } finish{Dull emission .8}} no_shadow}
           #end
           #local rix=rix+1;
       #end        
    #else
      #debug "You need to call ComputeDelta() function first. Do it now!\n"
    #end   
#end


#macro ComputeDelta(Smooth, Singular, sigma) // radius of spheres
#debug "Computing MSE... \n"         
   #declare EstNodes = array[NumPts][4]; // vector of estimated values [][x,y,z,est]
   #local rix=0;      
   #declare _de = 0;
   #while(rix<NumPts)
       #declare _zEst = RawDataPoint(<RNodes[rix].x, RNodes[rix].y, RNodes[rix].z>, Smooth, Singular, sigma); 
       #declare _de=_de+pow(RNodes[rix].z-_zEst, 2);
       #declare EstNodes[rix][0]=RNodes[rix].x; // real values
       #declare EstNodes[rix][1]=RNodes[rix].y; // real values
       #declare EstNodes[rix][2]=RNodes[rix].z; // mesured value of Z
       #declare EstNodes[rix][3]=_zEst; // computed, estimated value Z
       #local rix=rix+1;
   #end                     
   _de
#end

//2019-05-11: compute smoothed value at raw point "_indx", excluding!
#macro RawDataExPoint(_indx, Smooth, Singular, sigma)
         #local wghtSum = 0.0;
         #local wght = 0.00;
         #local ypov = 0.0;
         #local _rix  = 0;
         #local _P = RNodes[_indx];
         #while(_rix < NumPts) // take into account all points from input file
         // #debug concat(str(_rix, 10,0),"Pomidor\n")
           #if ( _rix != _indx )
             #local d  = sqrt(pow(_P.x-RNodes[_rix].x,2)+pow(_P.y-RNodes[_rix].y,2))/sigma; // distance between approximated and reference points ///corrected
             #local wght = (d>DZero?Smooth(d):Singular(d));
             #local wghtSum = wghtSum+wght;
             #local ypov = ypov+RNodes[_rix].z*wght;
           #else
           #end 
           #local _rix=_rix+1;
         #end
         #declare wghtSum = (wghtSum>0?wghtSum:DZero);   // just for stability
         #local ypov = ypov/wghtSum; // started 2019-05-11  // 
         ypov
#end

#macro ComputeResiduals(Smooth, Singular, sigma) // radius of spheres
#debug "Computing MSE... \n"         
   #declare EstNodes = array[NumPts][4]; // vector of estimated values [][x,y,z,est]
   #local _rixCR = 0;      
   #declare _de = 0;
   #while(_rixCR<NumPts)
       #if (mod((NumPts-_rixCR), 100) = 0 )
         #debug concat(str(NumPts-_rixCR,10,0), " residuals to compute.\n")
       #end
       #declare _zEst = RawDataExPoint(_rixCR, Smooth, Singular, sigma); 
       #declare _de=_de+pow(RNodes[_rixCR].z-_zEst, 2);
       #declare EstNodes[_rixCR][0]=RNodes[_rixCR].x; // real values
       #declare EstNodes[_rixCR][1]=RNodes[_rixCR].y; // real values
       #declare EstNodes[_rixCR][2]=RNodes[_rixCR].z; // mesured value of Z
       #declare EstNodes[_rixCR][3]=_zEst; // computed, estimated value Z
       #local _rixCR = _rixCR+1;
   #end                     
   _de
#end


/****M* BPatchSurf.inc/ScatterDataSurfaceExp()  [to be done]
* PURPOSE
*  ScatterDataSurfaceExp()
* SYNOPSIS
* INPUTS
* EXAMPLE
ScatterDataSurfaceExp()
DrawAllPatches()            // and then render it
* SEE ALSO
*  one should also refer to:
*   RawDataSurface(), MatrixFileSurface(), EvalPatchedSurface(), DrawPatch(), DrawPatch_UV(), DrawPatch(),
*   DrawAllPatches(), DrawAllPatchesGeo(), DrawAllPatchesWithTexture()
******/
#macro ScatterDataSurfaceExp(InFileName, x1, x2, Nxx, y1, y2, Nyy, power, sigma)
// Now open data file and import all points
   #fopen DataFile InFileName read
   #read(DataFile, DescriptX, DescriptY, DescriptZ)
   #read(DataFile, NumPts)
   #declare RNodes = array[NumPts];    // float vector: height of nodes AND x, y
   #declare RNodes[0] = <0.0, 0.0, 0.0>;
   #declare rix=0;

   #while(rix < NumPts)
      #read(DataFile, xx, yy, zz)
      #declare RNodes[rix] = <xx, yy, zz>; // real world coords
      #declare rix = rix + 1;
   #end
   #fclose DataFile
//==========================//

   #declare Nx=(Nxx>1?Nxx:2);     // at least 2 nodes
   #declare MaxNx=3*(Nx-1)+1;     // number of all nodes (cardinal+control) in 'x' direction

   #declare Ny=(Nyy>1?Nyy:2);     // at least 2 nodes
   #declare MaxNy=3*(Ny-1)+1;     // number of all nodes (cardinal+control) in 'y' direction

   #declare AllVertices = MaxNx*MaxNy;
   #declare PtV=array[AllVertices];       // vectors, real coords of all points
   #declare Pts=array[MaxNy][MaxNx];      // helper pointers to cardinal vectors PtV[]
   #declare Qds=array[Ny-1][Nx-1][4][4];  // array of integer pointers to vector, all patch cardinal&control points
// Initialization of ALL points

/// comment marked as ready to delete
//   #declare iy=0;
//   #while (iy<MaxNy)
//      #declare ix=0;
//      #while (ix<MaxNx)                   // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
//         #declare PtV[MaxNx*iy+ix] = <0, 0, 0>;
//         #declare Pts[iy][ix]= MaxNx*iy+ix;     // indices to the vector above
//         #declare ix=ix+1;
//      #end
//      #declare iy=iy+1;
//   #end
///^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

// Now initialize arrays and define Nodes array
   #local deltax = (x2-x1)/(MaxNx-1);
   #local deltay = (y2-y1)/(MaxNy-1);

   #declare iy = 0;
   #while (iy < MaxNy)
      #declare ix = 0;
      #while (ix < MaxNx) // !! left handed Pov Coord system: x=xpov, y=-zpov, z=y !!
         #local xpov = ix*deltax+x1; // x
         #local zpov = iy*deltay+y1; // y

         #declare PtV[MaxNx*iy+ix] = <zpov, 0, -xpov>;  // 'y' for cardinal points define later on crazy PovRay coords x>-z; y>x; z>y !
         #declare Pts[iy][ix] = MaxNx*iy+ix;            // indices to the vector above
         #declare ix = ix+1;
      #end
      #declare iy = iy+1;
   #end


   // I think it should be changed x<>y due to POvRay Coord System
   #declare SceneMaxX = x2;   #declare SceneMinX = x1;
   #declare SceneMaxY =0.0;   #declare SceneMinY =0.0;
   #declare SceneMaxZ =-y2;   #declare SceneMinZ =-y1;

//   #declare Nodes = array [Ny][Nx];
//   #declare Nodes[0][0] = 0.0;

// delta's for Qts matrix
   #declare iy=0;
   #while (iy<Ny)
      #declare ix=0;
      #while (ix<Nx)              // !! crazy left handed Pov Coord system: x=xpov, y=-zpov, z=y !! corrected: x=-zpow; y=xpov, z=ypow;
         #local PtVCurr = PtV[3*(MaxNx*iy+ix)];

         #local xpov =  PtVCurr.x; //   x
         #local zpov = -PtVCurr.z; // --y because I need temporary real world values

         #local ypov      = 0.0;
         #declare wghtSum = 0.0;  // sum of weights
         #declare wght    = 0.0;
         #local rix       = 0;
         #while(rix<NumPts) // now let's take into account all points from input file
            #declare d  = sqrt(pow(zpov-RNodes[rix].x,2)+pow(xpov-RNodes[rix].y,2))/sigma; // distance between approximated and reference points ///corrected
            #local wght = exp(-pow(d,power)); // specific weight
//#debug concat("d = ", str(d,10,7), "wght  ", str(exp(-pow(d, power)), 19, 17),"\n")

            #local ypov = ypov+RNodes[rix].z*wght;
            #declare wghtSum = wghtSum + wght;       // total weights
            #local rix=rix+1;
         #end

 //        #local ypov = ypov/NumPts;  // normalization
 #declare wghtSum = (wghtSum>0?wghtSum:1.0);   // just for stability
// #debug concat("wghtSum=", str(ypov,10,7), "\n")
//         #local ypov = VScale*ypov/wghtSum + VShift;
         #local ypov = ypov/wghtSum; // 2016-10-07
         #declare PtV[3*(MaxNx*iy+ix)] = <xpov, ypov, -zpov>; // -zpov sign change back to internal representation
// #declare PtV[MaxNx*iy+ix] = <xpov, ypov, -zpov>; // real vectorsw/points
//         #local PtVCurr = PtV[MaxNx*iy+ix];

//         #if (PtVCurr.x > SceneMaxX)
//            #declare SceneMaxX=PtVCurr.x;
//         #end
//         #if (PtVCurr.x < SceneMinX)
//            #declare SceneMinX=PtVCurr.x;
//         #end

         #if (ypov > SceneMaxY)
            #declare SceneMaxY=ypov;
         #end
         #if (ypov < SceneMinY)
            #declare SceneMinY=ypov;
         #end

/// comment marked as ready to delete
// left-handed coordinate system: Z-axis is reversed!!
//         #if (PtVCurr.z < SceneMaxZ)
//            #declare SceneMaxZ=PtVCurr.z;
//         #end
//         #if (PtVCurr.z > SceneMinZ)
//            #declare SceneMinZ=PtVCurr.z;
//         #end
/// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

         #declare ix=ix+1;
      #end
      #declare iy=iy+1;
   #end
   #declare ZCoeff=1/(SceneMaxY-SceneMinY);
// finally create all patches:
  EvalPatchedSurface()
// ready!
#end


#macro EvalPatchedSurface() // Evaluates data for func(x,y) patched bicubic surface
#declare iii = 0;
#while (iii<AllVertices)
   #declare PtV[iii]=<PtV[iii].x, PtV[iii].y - SceneMinY,PtV[iii].z>; // set zero base=> Min=0, Max=SceneMaxY-SceneMinY
   #declare iii=iii+1;
#end

#declare iy=0;
#while (iy<Ny-1)
   #declare ix=0;
   #while (ix<Nx-1)
      #declare py=0;
      #while (py<4)
         #declare px=0;
         #while (px<4)
            #declare Qds[iy][ix][py][px]=(3*iy+py)*MaxNx+3*ix+px; // indices to PtV=real vectors/points
            #declare px=px+1;
         #end
         #declare py=py+1;
      #end
      #declare ix=ix+1;
   #end
   #declare iy=iy+1;
#end

//=========================
// test #declare PtV[0]=<0,-.5,0>;
//=========================

#declare XCoeff=1/3; // this could be parametrized in the future
#declare YCoeff=1/3;
// A: Internal cross points

#declare ipy=1;
#while (ipy<Ny-1)
   #declare ipx=1;
   #while (ipx<Nx-1)
      // horizontal control points
      #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][0][3]]-PtV[Qds[ipy][ipx-1][0][0]])/2;
      #declare PtV[Qds[ipy][ipx-1][0][2]]=PtV[Qds[ipy][ipx][0][0]]-dXVec;
      #declare PtV[Qds[ipy][ipx  ][0][1]]=PtV[Qds[ipy][ipx][0][0]]+dXVec;

      // vertical control points
      #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][0]]-PtV[Qds[ipy-1][ipx][0][0]])/2;
      #declare PtV[Qds[ipy-1][ipx][2][0]]=PtV[Qds[ipy][ipx][0][0]]-dYVec;
      #declare PtV[Qds[ipy  ][ipx][1][0]]=PtV[Qds[ipy][ipx][0][0]]+dYVec;
      // next column
      #declare ipx=ipx+1;
   #end
   // next (horizontal) row
   #declare ipy=ipy+1;
#end

// B: boundary & skeleton control points
// 1: internal points only (NOT corners!)
// 1: horizontal 00->0X, Y0->YX
#declare ipy=0;
#declare ipx=1;
#while (ipx<Nx-1)
   #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][0][3]]-PtV[Qds[ipy][ipx-1][0][0]])/2;
   #declare PtV[Qds[ipy][ipx-1][0][2]]=PtV[Qds[ipy][ipx][0][0]]-dXVec;
   #declare PtV[Qds[ipy][ipx  ][0][1]]=PtV[Qds[ipy][ipx][0][0]]+dXVec;
   #declare ipx=ipx+1;
#end
#declare ipy=Ny-2;
#declare ipx=1;
#while (ipx<Nx-1)
   #declare dXVec=XCoeff*(PtV[Qds[ipy][ipx][3][3]]-PtV[Qds[ipy][ipx-1][3][0]])/2;
   #declare PtV[Qds[ipy][ipx-1][3][2]]=PtV[Qds[ipy][ipx][3][0]]-dXVec;
   #declare PtV[Qds[ipy][ipx  ][3][1]]=PtV[Qds[ipy][ipx][3][0]]+dXVec;
   #declare ipx=ipx+1;
#end

// 2: verticals (00->Y0, 0X->Yx)
#declare ipy=1;
#declare ipx=0;
#while (ipy<Ny-1)
   #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][0]]-PtV[Qds[ipy-1][ipx][0][0]])/2;
   #declare PtV[Qds[ipy-1][ipx][2][0]]=PtV[Qds[ipy][ipx][0][0]]-dYVec;
   #declare PtV[Qds[ipy  ][ipx][1][0]]=PtV[Qds[ipy][ipx][0][0]]+dYVec;
   #declare ipy=ipy+1;
#end
#declare ipy=1;
#declare ipx=Nx-2;
#while (ipy<Ny-1)
   #declare dYVec=YCoeff*(PtV[Qds[ipy][ipx][3][3]]-PtV[Qds[ipy-1][ipx][0][3]])/2;
   #declare PtV[Qds[ipy-1][ipx][2][3]]=PtV[Qds[ipy][ipx][0][3]]-dYVec;
   #declare PtV[Qds[ipy  ][ipx][1][3]]=PtV[Qds[ipy][ipx][0][3]]+dYVec;
   #declare ipy=ipy+1;
#end

// near boundary & corner points
// 1:corner 0-0=>0-X (Top)
// initial segment
#declare PtV[Qds[0][0][1][0]]=PtV[Qds[0][0][0][0]]+vnormalize(PtV[Qds[0][0][2][0]]-PtV[Qds[0][0][0][0]])
                                                    * vlength(PtV[Qds[1][0][0][0]]-PtV[Qds[0][0][2][0]]);

// final loop
#declare ipp=0;
#while (ipp<Nx-1)
   #declare PtV[Qds[0][ipp][1][3]]=PtV[Qds[0][ipp][0][3]]+vnormalize(PtV[Qds[0][ipp][2][3]]-PtV[Qds[0][ipp][0][3]])
                                                       * vlength(PtV[Qds[1][ipp][0][3]]-PtV[Qds[0][ipp][2][3]]);

   #declare ipp=ipp+1;
#end
// 0-0=>Y-0 (Left)
// initial segment
#declare PtV[Qds[0][0][0][1]]=PtV[Qds[0][0][0][0]]+vnormalize(PtV[Qds[0][0][0][2]]-PtV[Qds[0][0][0][0]])
                                                    * vlength(PtV[Qds[0][1][0][0]]-PtV[Qds[0][0][0][2]]);
// final loop
#declare ipp=0;
#while (ipp<Ny-1)
   #declare PtV[Qds[ipp][0][3][1]]=PtV[Qds[ipp][0][3][0]]+vnormalize(PtV[Qds[ipp][0][3][2]]-PtV[Qds[ipp][0][3][0]])
                                                       * vlength(PtV[Qds[ipp][1][3][0]]-PtV[Qds[ipp][0][3][2]]);
   #declare ipp=ipp+1;
#end


// 2:corner 0-X=>Y-X (Right)
// initial segment..
#declare PtV[Qds[0][Nx-2][0][2]]=PtV[Qds[0][Nx-2][0][3]]+vnormalize(PtV[Qds[0][Nx-2][0][1]]-PtV[Qds[0][Nx-2][0][3]])
                                                          * vlength(PtV[Qds[0][Nx-2][0][0]]-PtV[Qds[0][Nx-2][0][1]]);
// & final loop
#declare ipp=0;
#while (ipp<Ny-1)
#declare PtV[Qds[ipp][Nx-2][3][2]]=PtV[Qds[ipp][Nx-2][3][3]]+vnormalize(PtV[Qds[ipp][Nx-2][3][1]]-PtV[Qds[ipp][Nx-2][3][3]])
                                                          * vlength(PtV[Qds[ipp][Nx-2][3][0]]-PtV[Qds[ipp][Nx-2][3][1]]);
   #declare ipp=ipp+1;
#end

// 4:corner Y-0=>Y-X (Bottom)
// initial segment
#declare PtV[Qds[Ny-2][0][2][0]]=PtV[Qds[Ny-2][0][3][0]]+vnormalize(PtV[Qds[Ny-2][0][1][0]]-PtV[Qds[Ny-2][0][3][0]])
                                                          * vlength(PtV[Qds[Ny-2][0][0][0]]-PtV[Qds[Ny-2][0][1][0]]);
#declare ipp=0;
#while (ipp<Nx-1)
   #declare PtV[Qds[Ny-2][ipp][2][3]]=PtV[Qds[Ny-2][ipp][3][3]]+vnormalize(PtV[Qds[Ny-2][ipp][1][3]]-PtV[Qds[Ny-2][ipp][3][3]])
                                                             * vlength(PtV[Qds[Ny-2][ipp][0][3]]-PtV[Qds[Ny-2][ipp][1][3]]);
   #declare ipp=ipp+1;
#end

// internal control points
#declare ipy=0;
#while (ipy<Ny-1)
   #declare ipx=0;
   #while (ipx<Nx-1)
      // horizontal control points
      #declare HH=PtV[Qds[ipy][ipx][0][1]]-PtV[Qds[ipy][ipx][0][0]];
      #declare VV=PtV[Qds[ipy][ipx][1][0]]-PtV[Qds[ipy][ipx][0][0]];
      #declare PtV[Qds[ipy][ipx][1][1]]=PtV[Qds[ipy][ipx][0][0]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][3][1]]-PtV[Qds[ipy][ipx][3][0]];
      #declare VV=PtV[Qds[ipy][ipx][2][0]]-PtV[Qds[ipy][ipx][3][0]];
      #declare PtV[Qds[ipy][ipx][2][1]]=PtV[Qds[ipy][ipx][3][0]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][3][2]]-PtV[Qds[ipy][ipx][3][3]];
      #declare VV=PtV[Qds[ipy][ipx][2][3]]-PtV[Qds[ipy][ipx][3][3]];
      #declare PtV[Qds[ipy][ipx][2][2]]=PtV[Qds[ipy][ipx][3][3]]+HH+VV;

      #declare HH=PtV[Qds[ipy][ipx][0][2]]-PtV[Qds[ipy][ipx][0][3]];
      #declare VV=PtV[Qds[ipy][ipx][1][3]]-PtV[Qds[ipy][ipx][0][3]];
      #declare PtV[Qds[ipy][ipx][1][2]]=PtV[Qds[ipy][ipx][0][3]]+HH+VV;


      // next column
      #declare ipx=ipx+1;
   #end
   // next (horizontal) row
   #declare ipy=ipy+1;
#end

#end // macro

#macro FlatBiPatch(Vb1, Vb2, Vb3, Vb4) // Flat fourside figure
    bicubic_patch{
       type 1
       flatness 0.0
       u_steps 4 v_steps 4
       Vb1,                Vb1+(Vb2-Vb1)/3,       Vb1+(Vb2-Vb1)*2/3,             Vb2,
       Vb1+(Vb3-Vb1)/3,  Vb1+(Vb2-Vb1)/3+(Vb3-Vb1)/3,  Vb1+(Vb2-Vb1)*2/3+(Vb3-Vb1)/3,  Vb2+(Vb4-Vb2)/3
       Vb1+(Vb3-Vb1)*2/3,Vb1+(Vb3-Vb1)*2/3+(Vb2-Vb1)/3,Vb1+(Vb3-Vb1)*2/3+(Vb2-Vb1)*2/3,Vb2+(Vb4-Vb2)*2/3
       Vb3,              Vb3+(Vb4-Vb3)/3,            Vb3+ (Vb4-Vb3)*2/3,       Vb4
   }
#end

#macro FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2) // Flat fourside figure, c1, c2 Vb1-Vb4 control points defined elsewhere
    bicubic_patch{
       type 1
       flatness 0.0
       u_steps 4 v_steps 4
       Vb1,                c1,       c2,             Vb2,
       Vb1+(Vb3-Vb1)/3,  Vb1+(Vb2-Vb1)/3+(Vb3-Vb1)/3,  Vb1+(Vb2-Vb1)*2/3+(Vb3-Vb1)/3,  Vb2+(Vb4-Vb2)/3
       Vb1+(Vb3-Vb1)*2/3,Vb1+(Vb3-Vb1)*2/3+(Vb2-Vb1)/3,Vb1+(Vb3-Vb1)*2/3+(Vb2-Vb1)*2/3,Vb2+(Vb4-Vb2)*2/3
       Vb3,              Vb3+(Vb4-Vb3)/3,            Vb3+ (Vb4-Vb3)*2/3,       Vb4
   }
#end


#macro BSplinedBox() // standard box with a=1
#local Vb1=<0, 0,  0>;
#local Vb2=<1, 0,  0>;

#local Vb3=<0, 0, -1>;
#local Vb4=<1, 0, -1>;

// corners top side
#local Vt1=<0, 1,  0>;
#local Vt2=<1, 1,  0>;

#local Vt3=<0, 1, -1>;
#local Vt4=<1, 1, -1>;

union {
    FlatBiPatch(Vb1, Vb2, Vb3, Vb4)
    FlatBiPatch(Vt1, Vt2, Vt3, Vt4)

    FlatBiPatch(Vb1, Vt1, Vb3, Vt3)
    FlatBiPatch(Vb2, Vt2, Vb4, Vt4)

    FlatBiPatch(Vb1, Vt1, Vb2, Vt2)
    FlatBiPatch(Vb3, Vt3, Vb4, Vt4)

    texture { pigment{ color rgbft<.7, 1, .5, .3, .0> } finish { phong 1 reflection{ 0.00 metallic 0.00} }}
}
#end


#macro DrawBottomBlock(dcr) // 'dcr' additional depth of the base block
#local SceneMinYdcr=SceneMinY-dcr;
union {
    #for(ix,0,Nx-2,1)

        #local Vb1 = PtV[Qds[0][ix][0][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[0][ix][0][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr, Vb2.z>;

        #local c1 =  PtV[Qds[0][ix][0][1]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[0][ix][0][2]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end
    #for(iy,0,Ny-2,1)

        #local Vb1 = PtV[Qds[iy][0][0][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[iy][0][3][0]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[iy][0][1][0]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[iy][0][2][0]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end

    #for(ix,0,Nx-2,1)

        #local Vb1 = PtV[Qds[Ny-2][ix][3][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[Ny-2][ix][3][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[Ny-2][ix][3][1]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[Ny-2][ix][3][2]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end
    #for(iy,0,Ny-2,1)

        #local Vb1 = PtV[Qds[iy][Nx-2][0][3]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[iy][Nx-2][3][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[iy][Nx-2][1][3]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[iy][Nx-2][2][3]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end

    #local Vb1 = PtV[Qds[0][0][0][0]];
    #local Vb1 = <Vb1.x, SceneMinYdcr, Vb1.z>;

    #local Vb2 = PtV[Qds[0][Nx-2][0][3]];
    #local Vb2 = <Vb2.x, SceneMinYdcr, Vb2.z>;

    #local Vb3 = PtV[Qds[Ny-2][0][3][0]];
    #local Vb3 = <Vb3.x, SceneMinYdcr, Vb3.z>;

    #local Vb4 = PtV[Qds[Ny-2][Nx-2][3][3]];
    #local Vb4 = <Vb4.x, SceneMinYdcr, Vb4.z>;

    FlatBiPatch(Vb1, Vb2, Vb3, Vb4)
}
#end

#macro DrawBottomBlockFrom(Ystart) // depth of the base block - absolute value on Y axis
#local SceneMinYdcr=Ystart;    // DrawBottomBlock(SceneMinY-Ystart)
union {
    #for(ix,0,Nx-2,1)

        #local Vb1 = PtV[Qds[0][ix][0][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[0][ix][0][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr, Vb2.z>;

        #local c1 =  PtV[Qds[0][ix][0][1]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[0][ix][0][2]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end
    #for(iy,0,Ny-2,1)

        #local Vb1 = PtV[Qds[iy][0][0][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[iy][0][3][0]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[iy][0][1][0]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[iy][0][2][0]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end

    #for(ix,0,Nx-2,1)

        #local Vb1 = PtV[Qds[Ny-2][ix][3][0]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[Ny-2][ix][3][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[Ny-2][ix][3][1]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[Ny-2][ix][3][2]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end
    #for(iy,0,Ny-2,1)

        #local Vb1 = PtV[Qds[iy][Nx-2][0][3]]+<0,+SceneMinY,0>;
        #local Vb2 = PtV[Qds[iy][Nx-2][3][3]]+<0,+SceneMinY,0>;

        #local Vb3 = <Vb1.x, SceneMinYdcr, Vb1.z>;
        #local Vb4 = <Vb2.x, SceneMinYdcr,Vb2.z>;

        #local c1 =  PtV[Qds[iy][Nx-2][1][3]]+<0,+SceneMinY,0>;
        #local c2 =  PtV[Qds[iy][Nx-2][2][3]]+<0,+SceneMinY,0>;

        FlatBiPatch2(Vb1, Vb2, Vb3, Vb4, c1, c2)
    #end

    #local Vb1 = PtV[Qds[0][0][0][0]];
    #local Vb1 = <Vb1.x, SceneMinYdcr, Vb1.z>;

    #local Vb2 = PtV[Qds[0][Nx-2][0][3]];
    #local Vb2 = <Vb2.x, SceneMinYdcr, Vb2.z>;

    #local Vb3 = PtV[Qds[Ny-2][0][3][0]];
    #local Vb3 = <Vb3.x, SceneMinYdcr, Vb3.z>;

    #local Vb4 = PtV[Qds[Ny-2][Nx-2][3][3]];
    #local Vb4 = <Vb4.x, SceneMinYdcr, Vb4.z>;

    FlatBiPatch(Vb1, Vb2, Vb3, Vb4)
}
#end

#macro DrawRNodes(RR) // radius of spheres, default texture
#debug "doing DrawRNodes(RR).... \n"
   #local rix=0;
   #while(rix<NumPts)
       sphere {
         <RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x>, RR /// corrected 2016-10-07 !!       
//         <RNodes[rix].y, VScale*RNodes[rix].z, -RNodes[rix].x>, RR /// correction !!
/// org: <RNodes[rix].x, VScale*RNodes[rix].z, -RNodes[rix].y>, RR /// correction !!
//         texture{ pigment {color MandarinOrange }}
         no_shadow
         texture{ pigment {color VioletRed }}
       }
       #local rix=rix+1;
   #end
#end

#macro DrawRNodesT(RR) // RR radius of spheres
#debug "doing DrawRNodesT(RR).... \n"
   #local rix=0;
   #while(rix<NumPts)
       sphere {
         <RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x>, RR /// correction !!
/// org: <RNodes[rix].x, VScale*RNodes[rix].z, -RNodes[rix].y>, RR /// correction !!
         texture{ tmpTexture } // use first CommonDefs.SetTexture( FullTexture ) 
         #ifdef (Debug)
           #if (Debug>0)
            #debug "done.... \n"
           #end 
         #end  
       }
       #local rix=rix+1;
   #end
#end

#macro DrawRNodesGeo(RR) // RR radius of spheres - geometry only!!
   #local rix=0;
   #while(rix<NumPts)
       sphere {
         <RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x>, RR /// correction !!
       }
       #local rix=rix+1;
   #end
#end

#macro DrawRNodesGeoE(_RR, _Sc) // RR radius of spheres - geometry only!!
   #local rix=0;
   #while(rix<NumPts)
       sphere {
         0, _RR
         scale <1, _Sc, 1>
         translate <RNodes[rix].y, RNodes[rix].z, -RNodes[rix].x> /// correction !!
       }
       #local rix=rix+1;
   #end
#end


// helper macro, displays all points (Main & control) in the scene
#macro DrawAllPoints(R, rr)
#declare iy=0;
#while (iy<Ny-1)
   #declare ix=0;
   #while (ix<Nx-1)
      #declare py=0;
      #while (py<4)
         #declare px=0;
         #while (px<4)
            #if ((px=0 & py=0) | (px=3 & py =3)| (px=3 & py =0)| (px=0 & py =3)) // Main point
               sphere{
                  <PtV[Qds[iy][ix][py][px]].x,PtV[Qds[iy][ix][py][px]].y+SceneMinY,PtV[Qds[iy][ix][py][px]].z>, R
                     texture { pigment{ color rgb <clip(          PtV[Qds[iy][ix][py][px]].y, 0,1),
                                                   clip(     0.05*PtV[Qds[iy][ix][py][px]].x, 0,1),
                                                   clip(    1-    PtV[Qds[iy][ix][py][px]].y, 0,1)> }
                         finish { diffuse 0.9 phong 1 ambient 1.5}
                     } // end of texture
                  }
            #else                                                                // control point
               sphere{
                  <PtV[Qds[iy][ix][py][px]].x,PtV[Qds[iy][ix][py][px]].y+SceneMinY,PtV[Qds[iy][ix][py][px]].z>, rr
                     texture { pigment{ color rgb <.05,.05,.70> }
                         finish { diffuse 0.9 phong 1 ambient 1.5}
                     } // end of texture
                }
            #end
            #declare px=px+1;
         #end
         #declare py=py+1;
      #end
      #declare ix=ix+1;
   #end
   #declare iy=iy+1;
#end
#end // macro


// wersja 2011-06-07
#macro DrawColoredCardPoints(R) // Plots cardinal points only, pigment depends on Y-value of point
#debug  concat("DrawColoredCardPoints(R)", _FileVerMin)
   #declare ipy=0;
   #while (ipy<Ny-1)
      #declare ipx=0;
      #while (ipx<Nx-1)
//      #debug str(clip(1-0.1*PtV[Qds[ipy][ipx][0][0]].y, 0, 1),7,3)
         sphere {
              <PtV[Qds[ipy][ipx][0][0]].x, PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, PtV[Qds[ipy][ipx][0][0]].z>, R
/*              texture{ pigment{ color rgb <clip(     PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, 0, 1),
                                           clip(1-0.1*PtV[Qds[ipy][ipx][0][0]].x, 0, 1),
                                           clip(1.00-PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, 0, 1)>
                                }
                       }
                       finish { diffuse 0.9 phong 1 ambient .5}
*/
         }

// "external" points
        #if ((ipx=Nx-2) & (ipy<Ny-1))
           sphere {
                 <PtV[Qds[ipy][ipx][0][3]].x, PtV[Qds[ipy][ipx][0][3]].y+SceneMinY, PtV[Qds[ipy][ipx][0][3]].z>, R
           }
         #end

        #if ((ipx<Nx-1) & (ipy=Ny-2))
           sphere {
                 <PtV[Qds[ipy][ipx][3][0]].x, PtV[Qds[ipy][ipx][3][0]].y+SceneMinY, PtV[Qds[ipy][ipx][3][0]].z>, R
           }
         #end
// external=corner point
        #if ((ipx=Nx-2) & (ipy=Ny-2))
           sphere {
                 <PtV[Qds[ipy][ipx][3][3]].x, PtV[Qds[ipy][ipx][3][3]].y+SceneMinY, PtV[Qds[ipy][ipx][3][3]].z>, R
           }
         #end


         #declare ipx=ipx+1;
      #end
      // next (horizontal) row
      #declare ipy=ipy+1;
   #end
#end  // macro



#macro ExportGriddedData2File(Filename) // Plots cardinal points only, pigment depends on Y-value of point
#debug  concat("ExportGriddedData()", FileVer)
   #declare ipy=0;
   #while (ipy<Ny-1)
      #declare ipx=0;
      #while (ipx<Nx-1)
//      #debug str(clip(1-0.1*PtV[Qds[ipy][ipx][0][0]].y, 0, 1),7,3)
         sphere {
              <PtV[Qds[ipy][ipx][0][0]].x, PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, PtV[Qds[ipy][ipx][0][0]].z>, R
/*              texture{ pigment{ color rgb <clip(     PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, 0, 1),
                                           clip(1-0.1*PtV[Qds[ipy][ipx][0][0]].x, 0, 1),
                                           clip(1.00-PtV[Qds[ipy][ipx][0][0]].y+SceneMinY, 0, 1)>
                                }
                       }
                       finish { diffuse 0.9 phong 1 ambient .5}
*/
         }

// "external" points
        #if ((ipx=Nx-2) & (ipy<Ny-1))
           sphere {
                 <PtV[Qds[ipy][ipx][0][3]].x, PtV[Qds[ipy][ipx][0][3]].y+SceneMinY, PtV[Qds[ipy][ipx][0][3]].z>, R
           }
         #end

        #if ((ipx<Nx-1) & (ipy=Ny-2))
           sphere {
                 <PtV[Qds[ipy][ipx][3][0]].x, PtV[Qds[ipy][ipx][3][0]].y+SceneMinY, PtV[Qds[ipy][ipx][3][0]].z>, R
           }
         #end
// external=corner point
        #if ((ipx=Nx-2) & (ipy=Ny-2))
           sphere {
                 <PtV[Qds[ipy][ipx][3][3]].x, PtV[Qds[ipy][ipx][3][3]].y+SceneMinY, PtV[Qds[ipy][ipx][3][3]].z>, R
           }
         #end


         #declare ipx=ipx+1;
      #end
      // next (horizontal) row
      #declare ipy=ipy+1;
   #end
#end  // macro

#end
