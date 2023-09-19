/****E* ScPovPlot3D/GridnColorMap.pov
* PURPOSE                     
*   image:./Plot3dv3/imgs/GridnColorMap.png
*
*   Fig.[GridnColorMap] Example of coord system presented as wires (WiredBox() macro)
*   |html <hr width=50% align="left"> 
*     *********************************************************
*     **   Tested on PovRay 3.7                              **
*     **   License: GNU GPL                                  **
*     **   Homepage:    http://scpovplot3d.sourceforge.net   **
*     *********************************************************
*     **   version: 3.0.6 (& have a nice time ;)             **
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
****
  end of RoboDoc comment
****/
#version 3.7;
#declare _FileVerMaj ="3.0.6";
#declare _FileVerMin ="2018-04-06";
#declare _FileName   =input_file_name;                                

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")

// a lot of stony textures: T_Stone1 - T_Stone44
// Standard pre-defined colors
#include "math.inc"
#include "colors.inc"
#include "textures.inc"    
#include "stones1.inc"
#include "sunpos.inc"

#include "CommonDefs.inc"
#include "CoordsSys.inc"
#include "Cameras.inc"
//#include "WireSurf.inc"
//#include "Mesh2Surf.inc"
#include "TextExt.inc"
#include "BPatchSurf.inc"
//#include "Migrations.inc"
#include "ColorMaps.inc"

#ifndef (Method)   
   #declare Method=0; // Raw surface render
#end

// actual scene definition
//#declare Photons=true;
global_settings{
  ambient_light color rgb <.41,.41,.41> // sets an overall brightness/ambient light level in the scene
  max_trace_level 5                 // sets the maximum ray tracing bounce depth (1 or more) [5]
  assumed_gamma 2.0
  #if (Photons)                     // global photon block try to tune to Your needs
    photons {
      spacing 0.0002               // specify the density of photons
      media 10000
      count 100000               // alternatively use a total number of photons
      jitter 1.0                 // jitter phor photon rays
      max_trace_level 460         // optional separate max_trace_level
      adc_bailout 1/255          // see global adc_bailout
      autostop 0                 // photon autostop option
      radius 10                  // manually specified search radius
      expand_thresholds 0.2, 40  // (---Adaptive Search Radius---)
    }
  #end        
 
 #if (radio) 
  radiosity{
      pretrace_start 0.08
      pretrace_end   0.01
      count 150
      nearest_count 10
      error_bound 0.5
      recursion_limit 3
      low_error_factor 0.5
      gray_threshold 0.0
      minimum_reuse 0.005
      maximum_reuse 0.2
      brightness 1
      adc_bailout 0.005
  }         
  #end
}

#declare RGB0 = color_map {
          [ 0.0 rgbt < 1, 1, 1, 1>/3]
          [ 0.5 rgbt < 1, 1, 1, 1>/3]
          [ 1.0 rgbt < 1, 1, 1, 1>/3]
          };

#ifdef(bgr)
  background {rgbt bgr*<1,1,1,0>+<0,0,0,1>} // bgr - shade of gray
#else
  background {rgbt <.0150, .0150, .0150, 1>}
#end


/******************************************/
// InsertCartesian_LD(11,.2)
// left-handed(!) PovRay (x,y,z) -> Cartesian coordinate system (X,Y, Z) /right-handed
// change color of axes when needed
//  x - green => X 
//  y - blue  => Z
// -z - red   => Y       

// usage examples:
// EvalFuncPatchedSurf(SinCos3)
// FunctionSurface(SinCos3, 0, 4*pi, 20, 0, 4*pi, 20) // create PtV[], Pts[], Qds[] i.e. all surface patches: Func, xmin, xmax, Nx; ymin, ymax, Ny
// ScatterDataSurfaceExp("Saddle.dat", 0, 11, 10, 0, 11, 10, 2, 1)  // File/x1,x2,Nx/y1,y2,Ny,power,sigma, w(r0,r1)=exp( - (|r0-r1|/sigma)^power)

//==[ Info ]==================================
// #declare Newton  = function (d) { 1/d      }
// #declare Limiter = function (d) { 1/DZero  }        
// #declare Exponent= function (d) { exp(-d)  }
// #declare Gauss   = function (d) { exp(-pow(d,2))  }                                                      
// #declare SKrig   = function (d,pw) { 1/(1.0+pow(d,pw)) }  // SKrig(distance, power)        
//====[aliases]===
// #declare SKrig1  = function (d) { 1/(1.0+d) }        
// #declare SKrig2  = function (d) { 1/(1.0+pow(d,2)) }                                                             
// #declare SKrig4  = function (d) { 1/(1.0+pow(d,4)) }          
//===========================================

// The most configurable version
// Potential -> f(x,y) => smoothing potential, 1/r, 1/r^2, .., exp(-a*r), ...
// Structure of input file: 
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
// Now display everything
//#declare wykres = union {
//    DrawAllPatchesGeo()
//    DrawRNodes(.3)           
//    texture{  
//       InsertColorMap( false, 0) 
//       pigment{ gradient y 
//           color_map {
//              [0 color rgb 0]
//              [1 color rgb 1]
//              } 
//           }
//       scale <1, 1.02*(SceneMaxY-SceneMinY), 1> 
//       translate <0, SceneMinY-.01*(SceneMaxY-SceneMinY), 0>
//       finish { reflection .0 diffuse .5 emission .75 }
//   }                                                 
//}                    

// /*              

SetRGBFTColor(.3, .3, .5, .090, .31) // color of the walls RGB-Filter-Transmittance
SetInterior(inGlass)                 // set material: they looks glassy
SetTexture(Silver1)                  //txWhiteFT)                // set texture: see CommonDefs.inc                
#local _WL =-10.07;            
SetVScale(5)       
SetVShift(1E-3)
#if (clock_on)
  #declare _TimSTR = frame_number+2;
#else
  #declare _TimSTR = 12;
#end
#declare _TimSTR = 12;

#declare _Fmiu   = 0.7;
#declare _Ftemp  = 0.1;
#declare SKrigD  = function (d) { 1/(DZero+pow(d,4)) }          
#declare Gauss4  = function (d) { exp(-pow(d,4))  }                                                      
#declare Fermi   = function (d) { 1/(1+exp((d-_Fmiu)/_Ftemp)) }
#declare FCos    = function (d) { 0.5*(1+cos(  pi*d))      }
#declare FHann   = function (d) { 0.5*(1-cos(2*pi*d))      }

// frame mapping
#if (clock_on)
    #switch(frame_number)
    #range(1,5)    // 1
        #declare kernel = 6;
        #local f_n = mod(frame_number,6);
        #break
    
    #range(6,10)   // 2      
        #declare kernel = 7;
        #local f_n = mod(frame_number-5,6);
        #break
    
    #range(11,15)  // 3
        #declare kernel = 8;
        #local f_n = mod(frame_number-10,6);
        #break
   
    #range(16,20)  // 4
        #declare kernel = 5;
        #local f_n = mod(frame_number-15,6);
        #break

    #range(21,25)  // 4
        #declare kernel = 9;
        #local f_n = mod(frame_number-20,6);
        #break
    
    #range(26,30)  // 4
        #declare kernel = 10;                
        #local f_n = mod(frame_number-25,6);
        #break
    
    #else       
        #debug ""  
    #end    
#else
  #local kernel = 3; // the best?
  #local f_n = 1;
#end

#declare kernel =8;
#local f_n = 1;//frame_number;
// kernel choice
#switch(kernel)
    #case(1) #local _smth    = pow(10, f_n-4);      //  10/pow(2,div(frame_number, 6)); 
             #local KernName = "Gauss";    
             #local KernFun  = Gauss; 
             #local KernLim  = Gauss;  
             #break  // (frame_number>1?frame_number*0.01:0.01), // 7/25*10, //.005+clock*10, 

    #case(2) #local _smth = pow(5, f_n-4); 
             #local KernName = "Gauss4";
             #local KernFun  = Gauss4; 
             #local KernLim  = Gauss4;  
             #break             

    #case(3) #local _smth = pow(5, f_n-6); 
             #local KernName = "KrigD";      // 0.00001, //(frame_number>1?frame_number*0.05:0.001), // 7/25*10, //.005+clock*10,
             #local KernFun  = SKrigD; 
             #local KernLim  = SKrigD;  
             #break

    #case(4) #local _smth = pow(5, f_n-6); 
             #local KernName = "Krig4";    
             #local KernFun  = SKrig4; 
             #local KernLim  = SKrig4;  
             #break  // 0.00001, // (frame_number>1?frame_number*0.05:0.001), // 7/25*10, //.005+clock*10, 

    #case(5) #local _smth = 2001+(f_n-3)*500;          
             #local KernName = "Newton2";  
             #local KernFun  = Newton2; 
             #local KernLim  = Limiter2;  
             #break  // [N4->range 0-2000], (frame_number>1?frame_number*0.001:0.00001), // 7/25*10, //.005+clock*10,

    #case(6) #local _smth = 2001+(f_n-3)*500;          
             #local KernName = "Newton4";  
             #local KernFun  = Newton4; 
             #local KernLim  = Limiter4;  
             #break  // [N4->range 0-2000], (frame_number>1?frame_number*0.001:0.00001), // 7/25*10, //.005+clock*10,

    #case(7) #local _smth = pow(5, f_n-3);         
             #local KernName = "Exponent";
             #local KernFun  = Exponent; 
             #local KernLim  = Exponent;  
             #break  // 0.004 (frame_number>1?frame_number*0.001:0.001), // 7/25*10, //.005+clock*10,    

    #case(8) #local _smth = 0.7; // f_n*0.01+0.01; //(f_n>3?(f_n-3)*0.2-0.1:0.025*f_n); //pow(10, f_n-7);         
             #local KernName = "Fermi";
             #local KernFun  = Fermi; 
             #local KernLim  = Fermi;  
             #break  // 0.004 (frame_number>1?frame_number*0.001:0.001), // 7/25*10, //.005+clock*10,    

    #case(9) #local _smth = pow(10, f_n-6);         
             #local KernName = "FCos";
             #local KernFun  = FCos; 
             #local KernLim  = FCos;  
             #break  // 0.004 (frame_number>1?frame_number*0.001:0.001), // 7/25*10, //.005+clock*10,    

   #case(10) #local _smth = pow(10, f_n-7); 
             #local KernName = "Krig2";
             #local KernFun  = SKrig2; 
             #local KernLim  = SKrig2;  
    #else                 
             #debug "Godzilla is coming..."
#end                                     

#local _NGrid = 2;

#switch (Method)
#case (1)                                          
    RawDataSurface("UroczyskoHF.dat", 
        KernFun, KernLim, _smth,
        -65.92,  0.48, _NGrid*11,     // 8*frame_number,   // real minX, maxX (vertical)
         -1.36, 82.54, _NGrid*14      //10*frame_number)   // real minY, maxY (horizontal)    
    )                                              
    #declare SurfError = 0;//ComputeDelta(KernFun, KernLim, _smth );
    #declare SurfError = 0;//SurfError/NumPts; // MSE 
    #declare _Ts = concat("KDE= ", KernName, ", smth= ",TrimStr(str(_smth,12,8)),", MSE= ", TrimStr(str(SurfError,20,17)), ", N_G=",TrimStr(str(Nx*Ny,5,0)),", daytime: ", TrimStr(str(_TimSTR,2,0)),":00  ") 
    #debug _Ts
    #declare wykres = union {
      DrawAllPatchesGeo() 
//      DrawRNodes(.2)          
        merge{
           DrawBottomBlock(1)       
           texture{
//              Silver1
              pigment{color rgb .3 } //<0,1,0>
              finish{ Dull }
           }
        }
      no_reflection  
    }                    

/*
    merge{
      DrawColoredCardPoints(.3)          
      texture{pigment{color rgb <.7,.7, 0>} finish{Dull}}
      translate<1.36, 0, 0.48>
    }  
*/
//     merge{                     
//       DrawDeltaBW(Newton2, Limiter2, _smth, .1, 15 )
// //      texture{pigment{color rgb <.7,.7, 0>} finish{Dull}}
//       translate<1.36, 7, 0.48>
//       no_shadow
//     }  

// /*
// intersection{
    object { wykres 
        translate<1.36, 0, 0.48>
        scale<1/(82.54+1.36),1,1/(0.48+65.92)>
        rotate<-90,0,0>        
        texture{ 
            pigment{image_map{ png "UroczyskoHDCM1.png"}}
          // finish{ Dull }
        } // end of texture    

//       texture{ 
//           pigment{color rgb .5}
//           finish{ Dull }
//       } // end of texture    

        rotate<90,0,0>                        
        scale<(82.54+1.36),1,(0.48+65.92)>
        no_shadow
    }      

//     #if (clock_on & Variant<0)
//        box{<(frame_number-1)*5, -10, -100>, <(frame_number-1)*5+2, 10, 100>}
//     #else
//        box{<45, -10, -100>, <50, 10, 100> texture{pigment{color rgbt 1}}}    
//     #end   
// }
// */      
    #local _alfx = -95;
    #local _thex =  31;
    #local _dstx =  100;
    #local _angx =  55;
    #break
#case (2)
    #local HejMap = "UroczyskoHFG.png";
    #declare wykres = intersection {        
            height_field{ png HejMap smooth water_level _WL
                rotate<-90,0,0>        
                translate <0.0, 0.035, 0>
                texture{ 
                    pigment{image_map{ png "UroczyskoHDCM1.png"}}
                    finish{ Dull }
                } // end of texture
            } // end of height_field ----------------------------------
            box{<.00001,.035001,2> <.99999,.99999,-1> 
               texture{T_Copper_3D finish{scDullMirror emission .30}}
    //           texture{pigment{color rgb .5}}
            } // T_Copper_1A}} T_Silver_3A Red_Marble            
            translate <0.0, -0.035, 0>
            rotate<90,0,0>
            scale <1.36+82.54, 5, 65.92+0.48>
        } 
    #declare _Ts = concat("Height map, time: ", TrimStr(str(_TimSTR,2,0)),":00  "); //" [      Mean Sq. Error = ");//, TrimStr(str(SurfError,12,7)), "     ]")
    #debug _Ts
    object { wykres }        
    #local _alfx = -95;
    #local _thex =  31;
    #local _dstx =  100;
    #local _angx =  55;       
    #break
#else    
#end
// */
//////////////////////////////////
/// ]]]]]]]]]] EPILOG [[[[[[[[[[[

#ifdef (_alfx)                                                         
   #local _alf = _alfx;  // azimuth measured from -<Z>_p axis
#else
   #local _alf = -15; // 81;  //clock*45+81.0;  // azimuth measured from -<Z>_p axis
#end
#local alf_ =_alf;       
       
#ifdef (_thex)
   #local _the = _thex; // 57.0;//;      // elevation relative to Z-X plane
#else
   #local _the = 25;      
#end
#local the_ = _the; // 57.0;//;      // elevation relative to Z-X plane

#ifdef(_dstx)
    #local _dst = _dstx;  // specific distance from observed point
#else
    #local _dst = 50.0;   // common distance from observed point
#end

#ifdef (_angx)                   
  #local _ang =  _angx;
#else
  #local _ang =  40;                   
#end

#declare txWhite = texture{pigment{color rgbt<1,1,1,.2>}}

#declare TPmi=min_extent(wykres);                                                          
#declare TPma=max_extent(wykres);
#declare TP=(TPmi+TPma)/2;                                                          
SetCameraTarget(TP.x, TP.y-10, TP.z)           
// SetCameraTarget(TP.x, 0, 0)           
// SetCameraShift(<0, 60*(1-clock)+.5, 15>)
PrepareCamera( _alf, _the, _dst)
SetCameraAngle(_ang)   
//SetOrthographic(68.4) // 66.4 +W1000 +H776// +W650 +H582// +H1070 +W1352
SetPerspective()
CameraOn(1)     
//IntelligentEyeT(-15, 35.0, 20)       
SetLabelWidth(3.5)
//SetRGBMap(RGB5)
SetRGBMap(Cartographic_Colors)

#ifdef (_Ts)
    InsertBarKeyFrItemBelow ( TrimStr(_Ts), 
                              txMica  //txMica //,txWhiteFT //,T_Copper_3C  // Polished_Chrome
                              ,.09 
                              ,.05 
                              ,.35 //34      
                              ,texture { Polished_Chrome finish { Dull phong 1 }} // end of texture
                              ,.03)   //.02
#else
    // InsertBarKeyFrItem ( "Scale", txMica, .08, .04, .265, .002)
#end

// Final render - do not render for height map
// InsertCartesianArrows_LD(100, .6, .5)   

#if (Method>0)
// /*
   light_source{ SunPos(2018, 6, 21, _TimSTR, 00, 18, 53.655752, 18.2851338) rgb .9 }

   light_source {< 100, -10,  30> color rgb<.37, .37, .37> shadowless}   // natural light source and shadowless 2
   light_source {< -20, -10,  30> color rgb<.37, .37, .37> shadowless}   // natural light source and shadowless 2
   light_source {<  40, -10,  80> color rgb<.37, .37, .37> shadowless}   // natural light source and shadowless 2
   light_source {<  40, -10, -20> color rgb<.37, .37, .37> shadowless}   // natural light source and shadowless 2

// light_source {<-10, 100, -10> color rgb<.31, .31, .31> }   // natural light source and shadowless 2
// light_source {<100, 100,  50> color rgb<.37, .37, .37> shadowless }     // natural light source and shadowless 2
// light_source { <TP.x,0,TP.z>          
//                 color rgb <.248, .248, .248>    // light's color 
//                 area_light                // kind of light source (Art der Lichtquelle)
//                 <TPma.x-TPmi.x+20, 0, 0> <0, 0, TPma.z-TPmi.z-20>    // lights spread out across this distance (x * z)
//                 9, 6                     // total number of lights in grid (4x*4z = 16 lights)
//                 adaptive 2                // 0,1,2,3...
//                 jitter                    // adds random softening of light
//                 circular
//                 translate <0, 50, 0>      // <x,y,z> position of light
// } //--- end of area_light ---//
// */           
#end    
background {rgbt <1,1,1,0.99>}