/****h* ScPovPlot3D/Potential.inc
* PURPOSE
*   3D visualization of scalar fields (Coulomb & Lenard-Jones, grid defined. Partially done - any externally calculated on the grid.) using isosurfaces & other techniques.
*     image:./imgs/Potential-LJ-graphene.png
*
*     image:./imgs/Potential-H2O-01.jpg
*     
*   Fig.[Potential] Examples of visualisation of coulombean potential around the water molecule
*   |html <hr width=50% align="left">        
* VERSION
*  4.0.0, tested on PovRay 3.7.
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
*  2023-08-30, correction of debugging levels, CommonDefs.DebugMsg() added  
*              many enhancements, added: DrawInterpolGrdSrfSmoothed() and ComputeGridGradient()
*              from now gradient value is interpolated using full trilinear implementation
*  2016-01-05 - Tricubic interpolation, KDE interpolation;
*  2016-02-03 - inserted procedures for isosurface interpolation;
*  2023-08-23 - DrawInterpolGrdSrfSmoothed added(), documentation improved;
****
  end of RoboDoc comment
****/

#version 3.7;
#ifndef(_Potential_INC)
#declare _Potential_INC = "2023-09-15"; // last editing date        

#declare _FileName   = "Potential.inc";
#declare _FileVerMaj = "4.0.1";
#declare _FileVerMin = _Potential_INC;
#local _ModuleDescription = concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n");
#debug _ModuleDescription;

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end

// a lot of stone textures: T_Stone1 - T_Stone44
// Standard pre-defined colors
#include "math.inc"
#include "colors.inc"
#include "textures.inc"    
#include "stones1.inc"     
#include "metals.inc"     
#include "shapes.inc"

#include "CommonDefs.inc"
#include "scFinish.inc"
#include "Cameras.inc"
#include "Structures.inc"

// some useful constants
#declare epsil  = 8.8542;//E-6;  // E-12;    // przeskalowana przenikalnoœæ dielektryczna pró¿ni
#declare echarg = 1.602; //E-11;  // E-19;   // przeskalowany ³adunek elementarny
#declare qeff = 1E3*echarg/(4*pi*epsil);     // e/(4*pi*epsi_0)*1E10
#debug concat("Results are computed in [V] units, until scaling factor is used in selected macros. \n")
#debug "Structures should be defined using [\u00C5](angstrem) and multiplicity of elementary charge."
#debug "Thus, for q=1(e), distance R=1(A), potential is 14,3980381533 [V]\n"
//[x] V(R)=qeff * n/R; qeff=1/(4*pi*8.85E-12 [F/m])*1.602E-19 [C]* (1/(1E-10)) [m]

// global identifiers required by InterCubic() function
#declare Qmmm=0;	#declare  Q0mm=0;	#declare  Q1mm=0;	#declare  Q2mm=0;
#declare Qm0m=0;	#declare  Q00m=0;	#declare  Q10m=0;	#declare  Q20m=0;
#declare Qm1m=0;	#declare  Q01m=0;	#declare  Q11m=0;	#declare  Q21m=0;
#declare Qm2m=0;	#declare  Q02m=0;	#declare  Q12m=0;	#declare  Q22m=0;
			
#declare Qmm0=0;	#declare  Q0m0=0;	#declare  Q1m0=0;	#declare  Q2m0=0;
#declare Qm00=0;	#declare  Q000=0;	#declare  Q100=0;	#declare  Q200=0;
#declare Qm10=0;	#declare  Q010=0;	#declare  Q110=0;	#declare  Q210=0;
#declare Qm20=0;	#declare  Q020=0;	#declare  Q120=0;	#declare  Q220=0;
			
#declare Qmm1=0;	#declare  Q0m1=0;	#declare  Q1m1=0;	#declare  Q2m1=0;
#declare Qm01=0;	#declare  Q001=0;	#declare  Q101=0;	#declare  Q201=0;
#declare Qm11=0;	#declare  Q011=0;	#declare  Q111=0;	#declare  Q211=0;
#declare Qm21=0;	#declare  Q021=0;	#declare  Q121=0;	#declare  Q221=0;
			
#declare Qmm2=0;	#declare  Q0m2=0;	#declare  Q1m2=0;	#declare  Q2m2=0;
#declare Qm02=0;	#declare  Q002=0;	#declare  Q102=0;	#declare  Q202=0;
#declare Qm12=0;	#declare  Q012=0;	#declare  Q112=0;	#declare  Q212=0;
#declare Qm22=0;	#declare  Q022=0;	#declare  Q122=0;	#declare  Q222=0;
//===========================================================================
/****M* Potential.inc/SetExLimiter, _exx
* PURPOSE
*  Parameter for elementary cell selector. Default '0'. The greater the larger tolerance. For Catmul-Rom values >0 impose risk of artifacts. 
* SYNOPSIS
*/
#ifndef (_exx)
  #declare _exx = 0.0;
#end
#macro SetExxLimiter(_sx)
/*
* INPUTS
*  _sx - double - parameter value
* OUTPUTS
*  sets parameter 
* SEE ALSO
*  DrawCatRomSrf (), crSm0 (), crS12 ()
*
******/
  #declare _exx = _sx;
#end

/****M* Potential.inc/SetTanTau, _TanTau
* PURPOSE
*  Parameter for tangent calculations in Catmul-Rom formulation
* SYNOPSIS
*/
#ifndef (_TanTau)
  #declare _TanTau = 0.5;
#end  
#macro SetTanTau(_tt)
/*
* INPUTS
*  _tt - double - parameter value
* OUTPUTS
*  sets parameter 
* SEE ALSO
*  DrawCatRomSrf (), crSm0 (), crS12 ()
*
******/
  #declare _TanTau = _tt;
#end

#declare _gMin  = 0.0; // [mV/A] gradient range 
#declare _gMin2 = 0.0; // [mV/A] gradient range 
#declare _gMax  = 1E3; // [mV/A] gradient range 

/****M* Potential.inc/SetGradMin
* PURPOSE
*  Sets parameter _gMin for gradient texture mapping
* SYNOPSIS
*/
#macro SetGradMin(_g)
/*
* INPUTS
*  _g - double - parameter value
* OUTPUTS
*  sets internal parameter _gMin 
* SEE ALSO
*  SetGradMin2 (), SetGradMax ()
*
******/
  #declare _gMin  = _g;    
#end

/****M* Potential.inc/SetGradMin2
* PURPOSE
*  Sets parameter _gMin for gradient texture mapping
* SYNOPSIS
*/
#macro SetGradMin2(_g)
/*
* INPUTS
*  _g - double - parameter value
* OUTPUTS
*  sets internal parameter _gMin2 
* SEE ALSO
*  SetGradMin (), SetGradMax ()
*
******/
  #declare _gMin2 = 0;
#end  

/****M* Potential.inc/SetGradMax
* PURPOSE
*  Sets parameter _gMin for gradient texture mapping
* SYNOPSIS
*/
#macro SetGradMax(_g)           
/*
* INPUTS
*  _g - double - parameter value
* OUTPUTS
*  sets internal parameter _gMin2 
* SEE ALSO
*  SetGradMin (), SetGradMin2 ()
*
******/
  #declare _gMax  = _g; // [mV/A] gradient range, tuned to THIS example; (1-clock)*5E3+clock*5E4; // 5E7;
#end  





// default value of isosurface texture
#declare _IsoTexture = texture {
   #declare cl = CHSL2RGB(<5, .0, .5, .6, 1.0>); // light gray opaque
   pigment{ color rgbt <cl.red, cl.green, cl.blue, 0>}
   finish { Dull }
}  

#macro SetIsoTexture( _IT) #declare _IsoTexture = _IT; #end

// internal isosurface coefficients ===========
#declare _MaxG   = 3.0;   #macro SetMaxGrad(_MG)  #declare  _MaxG =_MG; #end
#declare _Accur  = 0.1;   #macro SetAccuracy(_A)  #declare _Accur = _A; #end
// #declare _Accura = 0.001; #macro SetAccura(_Acr)  #declare _Accura = _Acr; #end // obsolete
// internal isosurfaces coefficients END]======


#macro PotentialMap(_V, _n, _d ) 
    plane{
      _n, _d
      pigment{ function{ _V(x,y,z) }
          color_map { 
          [ 0.00 rgbt < 0, 0, 0, .0>]
          [ 0.50 rgbt < 0, 0, 0, .4>]
          [ 0.50 rgbt < 1, 1, 1, .4>]
          [ 1.00 rgbt < 1, 1, 1, .0>]
          }     
       }
//       double_illuminate
       no_radiosity
    }
#end

#macro PotentialMapG(_V, _n, _d ) 
    plane{
      _n, _d
      pigment{ function{ _V(x,y,z) }
          color_map { _RGBMap  }     
       }
//       double_illuminate
       no_radiosity
    }
#end

#macro PotentialMapTxt(_V, _n, _d )    // based on texture map
    plane{
      _n, _d
      texture{ function{ _V(x,y,z) }
          texture_map { _RGBMap  }     
       }
//       double_illuminate
       no_radiosity
    }
#end

#macro PotentialLogMap(_V, _n, _d ) 
    plane{
      _n, _d
      pigment{ function {log(_V(x,y,z)) }
          color_map { 
          [ 0.00 rgbt < 0, 0, 0, .0>]
          [ 0.50 rgbt < 0, 0, 0, .4>]
          [ 0.50 rgbt < 1, 1, 1, .4>]
          [ 1.00 rgbt < 1, 1, 1, .0>]
          }     
       }
//       double_illuminate
       no_radiosity
    }
#end

#macro PotentialLines(_V, _n, _d, _m ) // exploratory function
    plane{
      _n, _d                        
      #declare _w = 1.0/_m;
      texture{
          pigment{ function {_V(x, y, z) }
              color_map {                   
                 [ 0.00 rgbt <.0, .0, .0, .0>]    //.4         
              #for(i,1,_m) 
                 [ (i-1)*_w rgbt <0, 0, 0, .0>]   //.4
                 [ i*_w     rgbt <1, 1, 1, .0>]   //.2
              #end                  
                 [ 1 rgbt<1, 1, 1, .0>] // .2 - in order to have boundary white strip
              } 
          }
       }    
//       double_illuminate
       no_radiosity
    }
#end

#macro PotentialLogLines(_V, _n, _d, _m ) // exploratory function
    plane{
      _n, _d                        
      #declare _w = 1.0/_m;
      texture{
          pigment{ function {log(_V(x, y, z)) }
              color_map {                   
                 [ 0.00 rgbt <.0, .0, .0, .4>]             
              #for(i,1,_m) 
                 [ (i-1)*_w rgbt <0, 0, 0, .4>]   
                 [ i*_w     rgbt <1, 1, 1, .20>]
              #end                  
                 [ 1 rgbt<1, 1, 1, .20>] // in order to have central grey strip
              } 
          }
      
       }    
//       double_illuminate
       no_radiosity
    }
#end

/****M* Potential.inc/CreateVFC
* PURPOSE
*  Coulombean potential from network of electric charges.
*  Results are computed in [V], for distance given in Angstrems, 
*  and charge given in 'e' units.
* SYNOPSIS
*/
#macro CreateVFC(_PTs) // 
/*
* INPUTS
*  garray - array defining structure, see Structures.inc
* OUTPUTS
*  function 
* SEE ALSO
*  CreateVFC2, CreateVLJ, CreateVLJ2
*
******/
#debug concat("result is given in ",str(qeff,17,11),"*[V] \n")
function(x,y,z){ // use: #declare VVV = CreateVFC( table)
  qeff*(_PTs[1][0]/sqrt(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2))
  #for(i, 2, _PTs[0][0]) 
     + _PTs[i][0]/sqrt(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2))
  #end 
  )
}
#end // CreateVF()

/****M* Potential.inc/CreateVFC2
* PURPOSE
*  Coulombean potential from network of electric charges.
*  Results are computed in [V], for distance given in Angstrems, 
*  and charge given in 'e' units.
* SYNOPSIS
*/
#macro CreateVFC2(_PTs, _ff) // Coulombean potential from network of electric charges
/*
* INPUTS
*  garray - array defining structure, see Structures.inc
*  float  - unit divider, ex.: use 1000 for [mV], and 0.001 for [kV]. 
*           Volatile setting, may be changed in future releases.
* OUTPUTS
*  function 
* SEE ALSO
*  CreateVFC, CreateVLJ, CreateVLJ2
*
******/
#debug concat("result is given in ", str(qeff,17,11)," [",str(1/_ff,17,11),"*V] \n")
function(x,y,z){ // use: #declare VVV = CreateVFC( table)
  _ff*qeff*(_PTs[1][0]/sqrt(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2))
  #for(i, 2, _PTs[0][0]) 
     + _PTs[i][0]/sqrt(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2))
  #end 
  )
}
#end // CreateVF()

                                                                  
#macro CreateVLJ(_PTs) // create Lenard-Jones potential function from network of neutral molecules given by structure table _PTs: 
                       // Q (ignored), x,y,z, radius [n][4], L-J radius r_m [n][5], potential well [n][6] 
    #if (_PTs[0][0]>0)       // need one charge at least!
        function(x,y,z){                                                                                                                                            
          _PTs[1][6]*( pow( _PTs[1][5] *_PTs[1][5]/(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2)), 6)-2*pow( _PTs[1][5]*_PTs[1][5]/(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2)), 3)) // Rm Lenarda-Jonesa

          #for(i, 2, _PTs[0][0]) 
              +  _PTs[i][6]*(pow( _PTs[i][5]*_PTs[i][5]/(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2)), 6)-2*pow( _PTs[i][5]*_PTs[i][5]/(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2)), 3)) // Rm Lenarda-Jonesa
          #end 
          }
    #else
        #declare VVV = function(x,y,z){0} // just for safety
    #end
#end // CreateVF() 


#macro CreateVLJ2(_PTs,ff) // create Lenard-Jones potential function from network of neutral molecules given by structure table _PTs: 
                       // Q (ignored), x,y,z, radius [n][4], L-J radius r_m [n][5], potential well [n][6] 
    #if (_PTs[0][0]>0)       // need one charge at least!
        function(x,y,z){                                                                                                                                            
          ff*(_PTs[1][6]*( pow( _PTs[1][5]*_PTs[1][5]/(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2)), 6)-2*pow( _PTs[1][5]*_PTs[1][5]/(pow(x-_PTs[1][1],2) + pow(y-_PTs[1][2],2) + pow(z-_PTs[1][3],2)), 3)) // Rm Lenarda-Jonesa

          #for(i, 2, _PTs[0][0]) 
              +  _PTs[i][6]*(pow( _PTs[i][5]*_PTs[i][5]/(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2)), 6)-2*pow( _PTs[i][5]*_PTs[i][5]/(pow(x-_PTs[i][1],2) + pow(y-_PTs[i][2],2) + pow(z-_PTs[i][3],2)), 3)) // Rm Lenarda-Jonesa
          #end 
          )}
    #else
        #declare VVV = function(x,y,z){0} // just for safety
    #end
#end // CreateVF() 

#macro MakeEquiPlane( _V, _Thrsh, _B1, _B2 ) // V function, threshold, texture
    isosurface{
        function { _V(x,y,z)}
        open
        contained_by { box{ _B1, _B2} }
            
        threshold  _Thrsh
        max_gradient _MaxG              
        evaluate _MaxG*.7, sqrt(1/.7), .7
        accuracy _Accur        
        all_intersections
        texture{ _IsoTexture }//quick_color <.5,.5,.5> }
    }
#end

/****M* Potential.inc/DrawQVBox
* PURPOSE
*  Draws box containing computed grid of values
*
* SYNOPSIS
*/
#macro DrawQVBox(_QVr)  // default look
/*
* INPUTS
*  _QVr - garray
* OUTPUTS
*  none
* SEE ALSO
*  DrawBones (), DrawAllCells ()
*
******/
#local _N1 = dimension_size(_QVr,1);
#local _N2 = dimension_size(_QVr,2);
#local _N3 = dimension_size(_QVr,3);

  merge{    // bottom face
    sphere{ <_QVr[0][0][0][2], _QVr[0][0][0][3], -_QVr[0][0][0][1]>, 2*rr 
        texture{ pigment{color rgb <0, 0, 0.8>}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[_N1-1][0][0][2], _QVr[_N1-1][0][0][3], -_QVr[_N1-1][0][0][1]>, rr 
        texture{ pigment{color rgb 0.25}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[0][_N2-1][0][2], _QVr[0][_N2-1][0][3], -_QVr[0][_N2-1][0][1]>, rr 
        texture{ pigment{color rgb 0.25}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[_N1-1][_N2-1][0][2], _QVr[_N1-1][_N2-1][0][3], -_QVr[_N1-1][_N2-1][0][1]>, rr 
        texture{ pigment{color rgb 0.25}} finish{scDullMirror ambient .7}}
    
    // upper face
    sphere{ <_QVr[0][0][_N3-1][2], _QVr[0][0][_N3-1][3], -_QVr[0][0][_N3-1][1]>, rr 
        texture{ pigment{color rgb 0.55}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[_N1-1][0][_N3-1][2], _QVr[_N1-1][0][_N3-1][3], -_QVr[_N1-1][0][_N3-1][1]>, rr 
        texture{ pigment{color rgb 0.55}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[0][_N2-1][_N3-1][2], _QVr[0][_N2-1][_N3-1][3], -_QVr[0][_N2-1][_N3-1][1]>, rr 
        texture{ pigment{color rgb 0.55}} finish{scDullMirror ambient .7}}
    
    sphere{ <_QVr[_N1-1][_N2-1][_N3-1][2], _QVr[_N1-1][_N2-1][_N3-1][3], -_QVr[_N1-1][_N2-1][_N3-1][1]>, 2*rr 
        texture{ pigment{color rgb <0.8,0,0>}} finish{scDullMirror ambient .7}}
    
    // X-X axis
    cylinder{<_QVr[   0][0][0][2], _QVr[   0][0][0][3], -_QVr[   0][0][0][1]>, 
             <_QVr[_N1-1][0][0][2], _QVr[_N1-1][0][0][3], -_QVr[_N1-1][0][0][1]> rc
                texture{ pigment{color rgb <.8,.2,.2>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[   0][_N2-1][0][2], _QVr[   0][_N2-1][0][3], -_QVr[   0][_N2-1][0][1]>, 
             <_QVr[_N1-1][_N2-1][0][2], _QVr[_N1-1][_N2-1][0][3], -_QVr[_N1-1][_N2-1][0][1]> rc
                texture{ pigment{color rgb <.8,.2,.2>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[   0][0][_N3-1][2], _QVr[   0][0][_N3-1][3], -_QVr[   0][0][_N3-1][1]>, 
             <_QVr[_N1-1][0][_N3-1][2], _QVr[_N1-1][0][_N3-1][3], -_QVr[_N1-1][0][_N3-1][1]> rc
                texture{ pigment{color rgb <.8,.2,.2>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[   0][_N2-1][_N3-1][2], _QVr[   0][_N2-1][_N3-1][3], -_QVr[   0][_N2-1][_N3-1][1]>, 
             <_QVr[_N1-1][_N2-1][_N3-1][2], _QVr[_N1-1][_N2-1][_N3-1][3], -_QVr[_N1-1][_N2-1][_N3-1][1]> rc
                texture{ pigment{color rgb <.8,.2,.2>}} finish{scDullMirror ambient .7}
    }
    
    // Y-Y axis
    cylinder{<_QVr[0][0][   0][2], _QVr[0][0][   0][3], -_QVr[0][0][   0][1]>, 
             <_QVr[0][0][_N3-1][2], _QVr[0][0][_N3-1][3], -_QVr[0][0][_N3-1][1]> rc
                texture{ pigment{color rgb <.2,.2,.9>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[_N1-1][_N2-1][   0][2], _QVr[_N1-1][_N2-1][   0][3], -_QVr[_N1-1][_N2-1][   0][1]>, 
             <_QVr[_N1-1][_N2-1][_N3-1][2], _QVr[_N1-1][_N2-1][_N3-1][3], -_QVr[_N1-1][_N2-1][_N3-1][1]> rc
                texture{ pigment{color rgb <.2,.2,.9>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[_N1-1][0][   0][2], _QVr[_N1-1][0][   0][3], -_QVr[_N1-1][0][   0][1]>, 
             <_QVr[_N1-1][0][_N3-1][2], _QVr[_N1-1][0][_N3-1][3], -_QVr[_N1-1][0][_N3-1][1]> rc
                texture{ pigment{color rgb <.2,.2,.9>}} finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[0][_N2-1][0][2], _QVr[0][_N2-1][   0][3], -_QVr[0][_N2-1][   0][1]>, 
             <_QVr[0][_N2-1][0][2], _QVr[0][_N2-1][_N3-1][3], -_QVr[0][_N2-1][_N3-1][1]> rc
                texture{ pigment{color rgb <.2,.2,.9>}} finish{scDullMirror ambient .7}
    }
             
    // xYP - axis         
    cylinder{<_QVr[0][0][0][2], _QVr[0][   0][0][3], -_QVr[0][   0][0][1]>, 
             <_QVr[0][_N2-1][0][2], _QVr[0][_N2-1][0][3], -_QVr[0][_N2-1][0][1]> rc
                  texture{ pigment{color rgb <.2,.9,.2>}}
                  finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[_N1-1][0][0][2],    _QVr[_N1-1][   0][0][3], -_QVr[_N1-1][   0][0][1]>, 
             <_QVr[_N1-1][_N2-1][0][2], _QVr[_N1-1][_N2-1][0][3], -_QVr[_N1-1][_N2-1][0][1]> rc
                  texture{ pigment{color rgb <.2,.9,.2>}}
                  finish{scDullMirror ambient .7}
    }
    
    cylinder{<_QVr[0][   0][_N3-1][2], _QVr[0][   0][_N3-1][3], -_QVr[0][   0][_N3-1][1]>, 
             <_QVr[0][_N2-1][_N3-1][2], _QVr[0][_N2-1][_N3-1][3], -_QVr[0][_N2-1][_N3-1][1]> rc
                  texture{ pigment{color rgb <.2,.9,.2>}}
                  finish{scDullMirror ambient .7}
    }
    cylinder{<_QVr[_N1-1][   0][_N3-1][2], _QVr[_N1-1][   0][_N3-1][3], -_QVr[_N1-1][   0][_N3-1][1]>, 
             <_QVr[_N1-1][_N2-1][_N3-1][2], _QVr[_N1-1][_N2-1][_N3-1][3], -_QVr[_N1-1][_N2-1][_N3-1][1]> rc
                  texture{ pigment{color rgb <.2,.9,.2>}}
                  finish{scDullMirror ambient .7}
    }
  } // merge  
#end         

/****M* Potential.inc/DrawBones
* PURPOSE
*  Draws all 64 nodes used by interpolation procedures. For illustration purposes only.
*
* SYNOPSIS
*/
#macro DrawBones (_dd, _xyz)  // default look
/*
* INPUTS
*  _dd  - double   - distance parameter, _dd > 0.
*  _xyz - 3Dvector - additional shift of the whole structure 
* OUTPUTS
*  none
* SEE ALSO
*  DrawQVBox (), DrawAllCells (), DrawBonesBW ()
*
******/
  #local _QQ = array[4]{-1*_dd, 0, _dd, 2*_dd};  
  merge{    // bottom face                    
    #for(ix,0,3)
      #for(iy,0,3)
        #for(iz,0,3)
            sphere{ <_QQ[ix], _QQ[iy], -_QQ[iz]>,  _dd/10 
                texture{ pigment{color rgb <0.2+iz*0.2, 0.2+ix*0.2, 0.2+iy*0.2>}} finish{scDullMirror emission .7}}
        #end
      #end  
    #end
    
    // R X-X red axis                                                                
    #for(ix,0,3)
      #for(iy,0,3)
         cylinder{<_QQ[ix], _QQ[iy], -_QQ[0]>, 
                  <_QQ[ix], _QQ[iy], -_QQ[3]> _dd/10/6
                     texture{ pigment{color rgb <.8,.2,.2>}} finish{scDullMirror emission .7}
         }
      #end
    #end  
    
    
    // R Y-Y green axis
    #for(iz,0,3)
      #for(iy,0,3)
         cylinder{<_QQ[0], _QQ[iy], -_QQ[iz]>, 
                  <_QQ[3], _QQ[iy], -_QQ[iz]> _dd/10/6
                     texture{ pigment{color rgb <.2,.9,.0>}} finish{scDullMirror emission .7}
         }
      #end
    #end  
    
             
    // R Z-Z - blue axis         
    #for(ix,0,3)
      #for(iz,0,3)
          cylinder{<_QQ[ix], _QQ[0], -_QQ[iz]>, 
                   <_QQ[ix], _QQ[3], -_QQ[iz]> _dd/10/6
                       texture{ pigment{color rgb <.2,.2,.9>}} finish{scDullMirror emission .7}
          }
      #end
    #end  
    
    box{
       <0,0,0>,<_dd, _dd, -_dd> 
      texture{ pigment{color rgbt <1,1,1,.1>}} finish{scDullMirror emission .7}
    }
    
    translate _xyz    
  } // merge  
#end         

/****M* Potential.inc/DrawBonesBW
* PURPOSE
*  Draws all 64 nodes used by interpolation procedures. For illustration purposes only.
*
* SYNOPSIS
*/
#macro DrawBonesBW (_dd, _xyz)  // default look
/*
* INPUTS
*  _dd  - double   - distance parameter, _dd > 0.
*  _xyz - 3Dvector - additional shift of the whole structure 
* OUTPUTS
*  none
* SEE ALSO
*  DrawQVBox (), DrawAllCells (), DrawBonesBW ()
*
******/
  #local _QQ = array[4]{-1*_dd, 0, _dd, 2*_dd};  
  #local _b0 = 0.00;
  #local _bd = 0.20;
  merge{    // bottom face                    
    #for(ix,0,3)
      #for(iy,0,3)
        #for(iz,0,3)
            sphere{ <_QQ[ix], _QQ[iy], -_QQ[iz]>,  
                #if((ix=2 | ix=1)&(iy=2 | iy=1)&(iz=2 | iz=1))
                    _dd/12 
                    texture{ pigment{color rgb <0,0.50,0>} finish{Dull emission .6}}
                #else
                    _dd/20                 
                    texture{ pigment{color rgb 0.2} finish{Dull emission .3}}
                #end
            }    
        #end
      #end  
    #end
    
    // R X-X red axis                                                                
    #for(ix,0,3)
      #for(iy,0,3)
         cylinder{<_QQ[ix], _QQ[iy], -_QQ[0]>, 
                  <_QQ[ix], _QQ[iy], -_QQ[3]> 
                      _dd/10/8        
                      texture{ pigment{color rgb _b0} finish{scDullMirror emission .2}}
         }
      #end
    #end  
    
    // R Y-Y green axis
    #for(iz,0,3)
      #for(iy,0,3)
         cylinder{<_QQ[0], _QQ[iy], -_QQ[iz]>, 
                  <_QQ[3], _QQ[iy], -_QQ[iz]> 
                      _dd/10/8        
                      texture{ pigment{color rgb _b0} finish{scDullMirror emission .2}}
        }
      #end
    #end  
             
    // R Z-Z - blue axis         
    #for(ix,0,3)
      #for(iz,0,3)
          cylinder{<_QQ[ix], _QQ[0], -_QQ[iz]>, 
                   <_QQ[ix], _QQ[3], -_QQ[iz]> 
                      _dd/10/8        
                      texture{ pigment{color rgb _b0} finish{scDullMirror emission .2}}
         }          
      #end
    #end  


    // R X-X red axis                                                                
    #for(ix,1,2)
      #for(iy,1,2)
         cylinder{<_QQ[ix], _QQ[iy], -_QQ[1]>, 
                  <_QQ[ix], _QQ[iy], -_QQ[2]> 
                      _dd/10/4        
                      texture{ pigment{color rgb .5} finish{scDullMirror emission .2}}
         }
      #end
    #end  
    
    // R Y-Y green axis
    #for(iz,1,2)
      #for(iy,1,2)
         cylinder{<_QQ[1], _QQ[iy], -_QQ[iz]>, 
                  <_QQ[2], _QQ[iy], -_QQ[iz]> 
                      _dd/10/4        
                      texture{ pigment{color rgb .5} finish{scDullMirror emission .2}}
        }
      #end
    #end  
             
    // R Z-Z - blue axis         
    #for(ix,1,2)
      #for(iz,1,2)
          cylinder{<_QQ[ix], _QQ[1], -_QQ[iz]>, 
                   <_QQ[ix], _QQ[2], -_QQ[iz]> 
                      _dd/10/4        
                      texture{ pigment{color rgb .5} finish{scDullMirror emission .2}}
         }          
      #end
    #end  

    
    box{
       <0,0,0>,<_dd, _dd, -_dd> 
      texture{ pigment{color rgbft <1,1,1,0,0>/3+<0,0,0,.3,0>}} finish{Dull emission .7}
      scale 1-20*Tiny
      translate 10*<Tiny, Tiny,-Tiny>
    }             
    
//    polygon{5, <_QQ[0], _QQ[0]>, <_QQ[3], _QQ[0]>, <_QQ[3], _QQ[3]>, <_QQ[0], _QQ[3]>, <_QQ[0], _QQ[0]>
//    polygon{5, <_QQ[1], _QQ[1]>, <_QQ[3], _QQ[1]>, <_QQ[3], _QQ[3]>, <_QQ[1], _QQ[3]>, <_QQ[1], _QQ[1]>
      polygon{5, <_QQ[1], _QQ[1]>, <_QQ[2], _QQ[1]>, <_QQ[2], _QQ[2]>, <_QQ[1], _QQ[2]>, <_QQ[1], _QQ[1]>
      texture{ pigment{color rgbt <1,1,1,1>/3}} finish{Dull emission .7}
      rotate -90*x
      translate <0,0,-_dd>
      }

//    polygon{5, <_QQ[0], _QQ[0]>, <_QQ[3], _QQ[0]>, <_QQ[3], _QQ[3]>, <_QQ[0], _QQ[3]>, <_QQ[0], _QQ[0]>
//    polygon{5, <_QQ[1], _QQ[1]>, <_QQ[3], _QQ[1]>, <_QQ[3], _QQ[3]>, <_QQ[1], _QQ[3]>, <_QQ[1], _QQ[1]>
      polygon{5, <_QQ[1], _QQ[1]>, <_QQ[2], _QQ[1]>, <_QQ[2], _QQ[2]>, <_QQ[1], _QQ[2]>, <_QQ[1], _QQ[1]>
      texture{ pigment{color rgbt <1,1,1,1>/3}} finish{Dull emission .7}
//      rotate -90*x
      translate <_dd,0,0>

    }
 
//    polygon{5, <_QQ[0], _QQ[0]>, <_QQ[3], _QQ[0]>, <_QQ[3], _QQ[3]>, <_QQ[0], _QQ[3]>, <_QQ[0], _QQ[0]>
//    polygon{5, <_QQ[1], _QQ[1]>, <_QQ[3], _QQ[1]>, <_QQ[3], _QQ[3]>, <_QQ[1], _QQ[3]>, <_QQ[1], _QQ[1]>
      polygon{5, <_QQ[1], _QQ[1]>, <_QQ[2], _QQ[1]>, <_QQ[2], _QQ[2]>, <_QQ[1], _QQ[2]>, <_QQ[1], _QQ[1]>
      texture{ pigment{color rgbt <1,1,1,1>/3}} finish{Dull emission .7}
      rotate 90*y         
      translate <0,_dd,0>      
    }
    
    translate _xyz    
  } // merge  
#end         



  
   //==========================================================//
  //                                                          //
 // show me the structure - visualization of the whole grid  //
//==========================================================//
#macro DrawAllCells(_QVr) // default look

    #local _N1 = dimension_size(_QVr,1);
    #local _N2 = dimension_size(_QVr,2);
    #local _N3 = dimension_size(_QVr,3);

    merge{                             
        // All QV nodes
        #for(in1,0,_N1-1)   
           #for(in2,0,_N2-1)
              #for(in3,0,_N3-1)                
                 sphere{ <_QVr[in1][in2][in3][2],_QVr[in1][in2][in3][3], -_QVr[in1][in2][in3][1]>, rr  
                    texture{ pigment{color rgb 0.8*<in1/_N1, in2/_N2, in3/_N3>+0.2}}
                    finish{scDullMirror ambient .7}
                 }
              #end
           #end
        #end                  
        
        // cylinders along xR - axis - RED
        union{
            #for(inz,0,_N3-1)
               #for(iny,0,_N2-1)
                        cylinder{<_QVr[0]    [iny][inz][2], _QVr[    0][iny][inz][3], -_QVr[    0][iny][inz][1]>, 
                                 <_QVr[_N1-1][iny][inz][2], _QVr[_N1-1][iny][inz][3], -_QVr[_N1-1][iny][inz][1]> rc
                        texture{ pigment{color rgb <.8,.2,.2>}}
                        finish{scDullMirror ambient .7}
                     }
               #end
            #end  
        }                
        
        
        // cylinders along yRr - axis - GREEN
        union{
            #for(inz,0,_N3-1)
               #for(inx,0,_N1-1)
                        cylinder{<_QVr[inx][    0][inz][2], _QVr[inx][    0][inz][3], -_QVr[inx][    0][inz][1]>, 
                                 <_QVr[inx][_N2-1][inz][2], _QVr[inx][_N2-1][inz][3], -_QVr[inx][_N2-1][inz][1]> rc
                        texture{ pigment{color rgb <.2,.9,.2>}}
                        finish{scDullMirror ambient .7}
                     }
               #end
            #end  
        }                
        
        // cylinders along zRr - axis - BLUE
        union{
            #for(iny,0,_N2-1)
               #for(inx,0,_N1-1)
                        cylinder{<_QVr[inx][iny][    0][2], _QVr[inx][iny][    0][3], -_QVr[inx][iny][    0][1]>, 
                                 <_QVr[inx][iny][_N3-1][2], _QVr[inx][iny][_N3-1][3], -_QVr[inx][iny][_N3-1][1]> rc
                        texture{ pigment{color rgb <.2,.2,.9>}}
                        finish{scDullMirror ambient .7}
                     }
               #end
            #end  
        }                
    } // merge
#end

// /*   
   //====================================//
  //  key interpolation function        //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare VV1 = function(sx, sy, sz,   // xP, yP, zP
                        zx, zy, zz,   // lower, left, front corner of elementary cell
                        Q1d, Q2d, Q3d, Q4d,  // loads on boottom nodes
                        Q1u, Q2u, Q3u, Q4u,  // loads on upper nodes
                        ddd, ddd3){ 
// new version
  (
      (// bottom hex face  
         (-Q1d*(-sz+zz-ddd)-Q3d*(-zz+sz)) *(zx+ddd-sx) //Q13d*(1-z) +..reversed z-axis!
         +
         (-Q2d*(-sz+zz-ddd)-Q4d*(-zz+sz)) *(sx-zx)    //Q24d*z 
      )*(zy+ddd-sy)
      +
      (// upper hex face
         (-Q1u*(-sz+zz-ddd)-Q3u*(-zz+sz)) *(zx+ddd-sx) //Q13u*(1-z) +..reversed z-axis!
         +
         (-Q2u*(-sz+zz-ddd)-Q4u*(-zz+sz)) *(sx-zx)    //Q24u*z 
      )*(sy-zy)
  )/ddd3

} 
// */

#declare _ident = function(x,y,z,id){id};

  //======================================================//
 //    Drawing facility, gradient for a whole structure  //
//======================================================//
#macro DrawInterpolSrf(_trsh, _dd) // _trsh - isosurface threshold
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"), 1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#declare _dd3 = pow(_dd,3);
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n"), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing
                #else                  
                    isosurface{
                      function{VV1( x,y,z,    // internal conversion from real to internal representation
                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],      // "root" of elementary cell
                                    
                                    // bottom face
                                    QVr[  ixn][  iyn][izn][0],   // Q1d = _Q000
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d = _Q010
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d = _Q100
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d = _Q110
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], 
                                    QVr[  ixn][iyn+1][izn+1][0],  
                                    QVr[ixn+1][  iyn][izn+1][0], 
                                    QVr[ixn+1][iyn+1][izn+1][0],
                                    _dd, _dd3
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <QVr[  ixn][  iyn][  izn][2], QVr[  ixn][  iyn][  izn][3], -QVr[  ixn][  iyn][  izn][1]>,     // "root" of elementary cell >,
                                 <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn+1][1]>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      texture{_IsoTexture}
                    }
                #end
            #end 
        #end 
    #end
#end


   //=======================================================//
  //    Drawing facility gradient implementation per cell, //
 //    simplified formulation                             //
//=======================================================//
#macro DrawInterpolGrdSrf1(_trsh, _dd) // _trsh - isosurface threshold
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"),1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

    #declare _dd3 = pow(_dd,3);
    #local _CellsFound = 0;
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                        QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else                  
                    #ifdef(_grd) #undef _grd #end
                    #local _grd = Gradient_Length( // for external gradient
                                           function{ VV1( x,y,z,    // internal conversion from real to internal representation
                                             QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],      // "root" of elementary cell
                                             
                                             // bottom face
                                             QVr[  ixn][  iyn][izn][0],   // Q1d = _Q000
                                             QVr[  ixn][iyn+1][izn][0],   // Q2d = _Q010
                                             QVr[ixn+1][  iyn][izn][0],   // Q3d = _Q100
                                             QVr[ixn+1][iyn+1][izn][0],   // Q4d = _Q110
                                             
                                             // upper face
                                             QVr[  ixn][  iyn][izn+1][0], 
                                             QVr[  ixn][iyn+1][izn+1][0],  
                                             QVr[ixn+1][  iyn][izn+1][0], 
                                             QVr[ixn+1][iyn+1][izn+1][0],
                                             _dd, _dd3)}, 
                                          (<QVr[ixn  ][iyn  ][izn  ][2], QVr[ixn  ][iyn  ][izn  ][3], -QVr[ixn  ][iyn  ][izn  ][1]>+
                                           <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn+1][1]>)/2);
                    // #local _grd = clamp(_grd,0,1);                       
                                           
                    isosurface{
                      function{VV1( x,y,z,    // internal conversion from real to internal representation
                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],      // "root" of elementary cell
                                    
                                    // bottom face
                                    QVr[  ixn][  iyn][izn][0],   // Q1d = _Q000
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d = _Q010
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d = _Q100
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d = _Q110
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], 
                                    QVr[  ixn][iyn+1][izn+1][0],  
                                    QVr[ixn+1][  iyn][izn+1][0], 
                                    QVr[ixn+1][iyn+1][izn+1][0],
                                    _dd, _dd3
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <QVr[  ixn][  iyn][  izn][2], QVr[  ixn][  iyn][  izn][3], -QVr[  ixn][  iyn][  izn][1]>,     // "root" of elementary cell >,
                                 <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn+1][1]>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      // texture{_IsoTexture}
                      texture{
                         pigment{ // function{_ident(x,y,z,_grd)} - identity function
                            function{ clip(log(_grd -_gMin+1), 0, log(_gMax-_gMin+1))/log(_gMax-_gMin+1) }           
                            color_map{_RGBMap}
                         }
                      }
                    }      
                    #local _CellsFound = _CellsFound+1;
                #end
            #end 
        #end 
        DebugMsg(concat(str(_CellsFound,10,0), " found to date.\n"), 1)    
    #end                                                               
    DebugMsg(concat(str(_CellsFound,10,0), " found in total.\n"), 0)
#end  // #macro DrawInterpolGrdSrf1()

  //======================================================//
 //    Drawing facility gradient implementation per cell //
//======================================================//
#macro DrawInterpolGrdSrf(_trsh, _dd) // _trsh - isosurface threshold
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"), 1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

    #declare _dd3 = pow(_dd,3);
    #local _CellsFound = 0;
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing

                #else        
                    #ifdef(_grd0) #undef _grd0 #end
                    #declare _grd0 = function(x,y,z){
                        VV1( x, y, z,    // internal conversion from real to internal representation
                            QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],      // "root" of elementary cell
                            
                            // bottom face
                            QVr[  ixn][  iyn][izn][0],   // Q1d = _Q000
                            QVr[  ixn][iyn+1][izn][0],   // Q2d = _Q010
                            QVr[ixn+1][  iyn][izn][0],   // Q3d = _Q100
                            QVr[ixn+1][iyn+1][izn][0],   // Q4d = _Q110
                            
                            // upper face
                            QVr[  ixn][  iyn][izn+1][0], 
                            QVr[  ixn][iyn+1][izn+1][0],  
                            QVr[ixn+1][  iyn][izn+1][0], 
                            QVr[ixn+1][iyn+1][izn+1][0],
                            _dd, _dd3)
                    }                                 
                    #ifdef(_grd) #undef _grd #end
                    #declare _grd = fn_Gradient(_grd0);
                    isosurface{
                       function{VV1( x,y,z,    // internal conversion from real to internal representation
                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],      // "root" of elementary cell
                                    
                                    // bottom face
                                    QVr[  ixn][  iyn][izn][0],   // Q1d = _Q000
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d = _Q010
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d = _Q100
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d = _Q110
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], 
                                    QVr[  ixn][iyn+1][izn+1][0],  
                                    QVr[ixn+1][  iyn][izn+1][0], 
                                    QVr[ixn+1][iyn+1][izn+1][0],
                                    _dd, _dd3
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <QVr[  ixn][  iyn][  izn][2], QVr[  ixn][  iyn][  izn][3], -QVr[  ixn][  iyn][  izn][1]>,     // "root" of elementary cell >,
                                 <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn+1][1]>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      // texture{_IsoTexture}
                      texture{
                         pigment{ // function{_ident(x,y,z,_grd)} - identity function
                            function{ clip(log(_grd(x,y,z ) -_gMin+1), 0, log(_gMax-_gMin+1))/log(_gMax-_gMin+1) }           
                            color_map{_RGBMap}
                         }
                         finish{ scDullMirror }
                      }
                    }      
                    #local _CellsFound = _CellsFound+1;
                #end
            #end 
        #end 
        DebugMsg(concat(str(_CellsFound,10,0), " found to date.\n"), 1)    
    #end         
    DebugMsg(concat(str(_CellsFound,10,0), " found in total.\n"), 0)    
#end // #macro DrawInterpolGrdSrf( )

// /*   
   //====================================//
  //  key interpolation function        //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare VGrid = function(
                        tx, ty, tz,              // tau xP=x/ddd, tau yP=y/ddd, tau zP=z/ddd
                        _Q000, _Q100, _Q110, _Q010,  // loads  //Q(ijk) refers to internal POVRay
                        _Q001, _Q101, _Q111, _Q011  // loads 
                 ){ 
// version on translated unary box
  ( // POVRay coordinates
     _Q000*(1-tx)*(1-ty)*( -tz)+
     _Q100*   tx *(1-ty)*( -tz)+
     _Q010*(1-tx)*   ty *( -tz)+
     _Q001*(1-tx)*(1-ty)*(1+tz)+
     _Q110*   tx *   ty *( -tz)+
     _Q101*   tx *(1-ty)*(1+tz)+
     _Q011*(1-tx)*   ty *(1+tz)+
     _Q111*   tx *   ty *(1+tz)
  )
} 

#declare VGridGrad = function(                       // elementary cube [1,1,1] at origin is used
                        tx, ty, tz,                  // txP=y/ddd, tyP=z/ddd, tzP=-x/ddd;
                        _Q000, _Q100, _Q110, _Q010,  // loads Q(ijk) are placed at internal POVRay
                        _Q001, _Q101, _Q111, _Q011,  // vertices of unary cube {<0,0,0> <1,1,-1>}
                        _ddd){ 
// algebraic version - corrected
// tx == yR; ty== zR; tz == -xR
  sqrt(
      pow( // d/dxp == d/dyr    
           (_Q100-_Q000)*(1-ty)*( -tz)+
           (_Q110-_Q010)*   ty *( -tz)+
           (_Q101-_Q001)*(1-ty)*(1+tz)+
           (_Q111-_Q011)*   ty *(1+tz)
      ,2)+
      pow( // d/dyp == d/dzr    
           (_Q010-_Q000)*(1-tx)*( -tz)+
           (_Q110-_Q100)*   tx *( -tz)+
           (_Q111-_Q101)*(1-tx)*(1+tz)+
           (_Q011-_Q001)*   tx *(1+tz)
      ,2)+ 

      pow( // d/dzp == -d/dxr    
           (_Q001-_Q000)*(1-tx)*(1-ty)+
           (_Q101-_Q100)*(  tx)*(1-ty)+
           (_Q111-_Q110)*(  tx)*(  ty)+
           (_Q011-_Q010)*(1-tx)*(  ty)
      ,2)  // '1' for testing
  )  /_ddd //unnecessary
} 

#declare VAvrgGrad = function(                       // average gradient at the center of elementary cell, 
                        tx, ty, tz,                  // txP=y/ddd, tyP=z/ddd, tzP=-x/ddd;
                        _Q000, _Q100, _Q110, _Q010,  // loads Q(ijk) are placed at internal POVRay
                        _Q001, _Q101, _Q111, _Q011,  // vertices of unary cube {<0,0,0> <1,1,-1>}
                        _ddd){ 
// algebraic version - corrected
// tx == yR; ty== zR; tz == -xR
  sqrt(
      pow( // d/dxp 
           (_Q100-_Q000+_Q110-_Q010+_Q101-_Q001+_Q111-_Q011)
      ,2)+
      pow( // d/dyp
           (_Q010-_Q000+_Q110-_Q100+_Q111-_Q101+_Q011-_Q001)
      ,2)+ 
      pow( // d/dzp    
           (_Q001-_Q000+_Q101-_Q100+_Q111-_Q110+_Q011-_Q010)
      ,2)  // '1' for testing
  ) /_ddd/4 
} 


//[============================================================
//[     Drawing facility gradient implementation per cell  ==
//[========================================================
/****M* Potential.inc/DrawInterpolGrdSrf2 
* PURPOSE
*  Draws estimated isosurface of scalar field over the computational grid
*
* SYNOPSIS
*/
#macro DrawInterpolGrdSrf2(_trsh, _dd) // _trsh - isosurface threshold, _dd - cell of the grid size
/*
* INPUTS
*  _trsh - double   - isosurface threshold fo scalar field
*  _dd   - double   - distance parameter, _dd > 0.
*  
* OUTPUTS
*  none
* SEE ALSO
*  DrawQVBox (),DrawInterpolGrdSrf (),DrawInterpolGrdSrf1 (), DrawInterpolGrdSrf2 (), DrawInterpolGrdSrfSmoothed () 
*
******/
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"), 1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

    #declare _dd3 = pow(_dd,3);
    #local _CellsFound = 0;
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                           ) 
                    | _trsh>max(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing

                #else        
                    isosurface{
                      function{VGrid( x, y, z,    // internal conversion from real to internal representation
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <0, 0, -1>,     // "root" of elementary cell >,
                                 <1, 1,  0>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      // texture{_IsoTexture}
                      texture{
                         pigment{ // 
                            function{ clip(
                                 // log(
                                 (VGridGrad(x,y,z, 
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0],
                                   _dd // scaling by _dd is neccessary, though (leftover from derivative: (d/dx)(1-x/_dd).
                                 )-_gMin)/(_gMax-_gMin), 0, 1) } 
                                 //) , 0, 
                                 //log(_gMax-_gMin+.0001))/log(_gMax-_gMin+.0001) }           
                                 // -_gMin+1), 0, log(_gMax-_gMin+1))/log(_gMax-_gMin+1) }           
                            color_map{_RGBMap}
                         }
                         finish{ scDullMirror }
                      }                                                                               
                      scale _dd
                      translate <QVr[ixn  ][iyn][izn][2], QVr[ixn  ][iyn][izn][3], -QVr[ixn  ][iyn][izn][1]>      // "root" of elementary cell
                    }      
                    #local _CellsFound = _CellsFound+1;
                #end
            #end 
        #end 
        DebugMsg(concat(str(_CellsFound,10,0), " found to date.\n"), 1)    
    #end                                                             
    DebugMsg(concat(str(_CellsFound,10,0), " found in total.\n"), 0)    
#end // #macro DrawInterpolGrdSrf2()

//[============================================================
//[     Drawing facility gradient implementation per cell  ==
//[========================================================
/****M* Potential.inc/DrawInterpolGrdSrf3 
* PURPOSE
*  Draws estimated isosurface of scalar field over the computational grid
*  gradient is average at the center of the cell
*
* SYNOPSIS
*/
#macro DrawInterpolGrdSrf3(_trsh, _dd) // _trsh - isosurface threshold, _dd - cell of the grid size
/*
* INPUTS
*  _trsh - double   - isosurface threshold fo scalar field
*  _dd   - double   - distance parameter, _dd > 0.
*  
* OUTPUTS
*  none
* SEE ALSO
*  DrawQVBox (),DrawInterpolGrdSrf (),DrawInterpolGrdSrf1 (), DrawInterpolGrdSrf2 (), DrawInterpolGrdSrfSmoothed () 
*
******/
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"), 1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

    #declare _dd3 = pow(_dd,3);
    #local _CellsFound = 0;
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                           ) 
                    | _trsh>max(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing

                #else        
                    isosurface{
                      function{VGrid( x, y, z,    // internal conversion from real to internal representation
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <0, 0, -1>,     // "root" of elementary cell >,
                                 <1, 1,  0>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      // texture{_IsoTexture}
                      texture{
                         pigment{ // 
                            function{ clip(
                                 // log(
                                 (VAvrgGrad(x,y,z, 
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0],
                                   _dd // scaling by _dd is neccessary, though (leftover from derivative: (d/dx)(1-x/_dd).
                                 )-_gMin)/(_gMax-_gMin), 0, 1) } 
                                 //) , 0, 
                                 //log(_gMax-_gMin+.0001))/log(_gMax-_gMin+.0001) }           
                                 // -_gMin+1), 0, log(_gMax-_gMin+1))/log(_gMax-_gMin+1) }           
                            color_map{_RGBMap}
                         }
                         finish{ scDullMirror }
                      }                                                                               
                      scale _dd
                      translate <QVr[ixn  ][iyn][izn][2], QVr[ixn  ][iyn][izn][3], -QVr[ixn  ][iyn][izn][1]>      // "root" of elementary cell
                    }      
                    #local _CellsFound = _CellsFound+1;
                #end
            #end 
        #end 
        DebugMsg(concat(str(_CellsFound,10,0), " found to date.\n"), 1)    
    #end                                                             
    DebugMsg(concat(str(_CellsFound,10,0), " found in total.\n"), 0)    
#end // #macro DrawInterpolGrdSrf3()



/****M* Potential.inc/ComputeGridGradient
* PURPOSE
*  Based on values of scalar field distributed over 3D grid, Nx*Ny*Nz nodes stored in 
*  GLOBAL QVr[Nx][Ny][Nz][5] array, computes 3D grid of equal size and extent 
*  containing values of (last index):
*  ..[0] - value of estimated gradient (ie. length of the gradient vector)
*  ..[1][2][3] - estimated components of the gradient vector: g_i = (v_(i+1)-v_(i-1))/(2d)   
*  ..[4] - control firld, not used yet.                      
*  produces array GVr[Nx][Ny][Nz][5].
*
* SYNOPSIS
*/
#macro ComputeGridGradient() 
/*
* INPUTS
*  QVr - garray, global.
* OUTPUTS
*  GVr - garray, global.
* SEE ALSO
*  VGrid (), VGridGrad (), DrawInterpolGrdSrfSmoothed (), DrawInterpolGrdSrf (), DrawInterpolGrdSrf1 (), DrawInterpolGrdSrf2 ()
*
******/
    DebugMsg( "Allocating internal Gradient array\n", 1)
    #ifndef (QVr)
      DebugMsg("Compute QVr[Nx][Ny][Nz][5] array first!\n",0)
      #local _halt = _QVr[0][0][0][0];
    #end  
    
    #declare GVr = array[dimension_size(QVr,1)]
                        [dimension_size(QVr,2)]
                        [dimension_size(QVr,3)][5]; // gradient VALUE [0] and vector ([1,2,3], [4] unused)!  
//    #declare GVr = array[_Nx][_Ny][_Nz][5]; // gradient VALUE [0] and vector ([1,2,3], [4] unused)!  
    

    #local  gTmp = 0.0;  // safety :[    
    //==============    
    // 0) 8 vertices                                 
    // 0_000/1
    DebugMsg(concat("processing vertice: 0-0-0\n"), 1)
    #local in1 = 0;
    #local in2 = 0;
    #local in3 = 0;
    // real coordinates!!
    #local _gx = (QVr[    1][    0][    0][0] - QVr[    0][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][    1][    0][0] - QVr[    0][    0][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][    1][0] - QVr[    0][    0][    0][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED 
    #local _Gmin = gTmp;  // 
    #local _Gmax = 0.0;   // <-as vector length canot be less than zero.
    #local MinMaxSet = false; // safety
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end

    // 0_111/2
    DebugMsg(concat("processing vertice: 1-1-1\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in2 = dimension_size(GVr,2)-1; 
    #local in3 = dimension_size(GVr,3)-1; 
    // real coordinates!!
    #local _gx = (QVr[in1  ][    0][    0][0] - QVr[in1-1][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][in2  ][    0][0] - QVr[    0][in2-1][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][in3  ][0] - QVr[    0][    0][in3-1][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
                                
    // 0_011/3
    DebugMsg(concat("processing vertice: 0-1-1\n"),1)
    #local in1 = 0;
    #local in2 = dimension_size(GVr,2)-1; 
    #local in3 = dimension_size(GVr,3)-1; 
    // real coordinates!!
    #local _gx = (QVr[in1+1][    0][    0][0] - QVr[in1  ][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][in2  ][    0][0] - QVr[    0][in2-1][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][in3  ][0] - QVr[    0][    0][in3-1][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end

    // 0_101/4
    DebugMsg(concat("processing vertice: 1-0-1\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in2 = 0; 
    #local in3 = dimension_size(GVr,3)-1; 
    // real coordinates!!
    #local _gx = (QVr[in1  ][    0][    0][0] - QVr[in1-1][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][in2+1][    0][0] - QVr[    0][in2  ][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][in3  ][0] - QVr[    0][    0][in3-1][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end

    // 0_110/5
    DebugMsg(concat("processing vertice: 1-1-0\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in2 = dimension_size(GVr,2)-1; 
    #local in3 = 0; 
    // real coordinates!!
    #local _gx = (QVr[in1  ][    0][    0][0] - QVr[in1-1][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][in2  ][    0][0] - QVr[    0][in2-1][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][in3+1][0] - QVr[    0][    0][in3  ][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end                       
    
    // 0_100/6
    DebugMsg(concat("processing vertice: 1-0-0\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;; // 0;
    #local in2 = 0;
    #local in3 = 0; // dimension_size(GVr,3)-1; 
    // real coordinates!!
    #local _gx = (QVr[in1  ][    0][    0][0] - QVr[in1-1][    0][    0][0])/ dd; 
    #local _gy = (QVr[    0][in2+1][    0][0] - QVr[    0][in2  ][    0][0])/ dd; 
    #local _gz = (QVr[    0][    0][in3+1][0] - QVr[    0][    0][in3  ][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    
    // 0_010/7
    DebugMsg(concat("processing vertice: 0-1-0\n"), 1)
    #local in1 = 0;
    #local in2 = dimension_size(GVr,2)-1; 
    #local in3 = 0; 
    // real coordinates!!
    #local _gx = (QVr[in1+1][    0][in3  ][0] - QVr[in1  ][in2  ][in3  ][0])/ dd; 
    #local _gy = (QVr[in1  ][in2  ][in3  ][0] - QVr[    0][in2-1][in3  ][0])/ dd; 
    #local _gz = (QVr[in1  ][    0][in3+1][0] - QVr[    0][in2  ][in3  ][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end

    // 0_001/8
    DebugMsg(concat("processing vertice: 0-0-1\n"), 1)
    #local in1 = 0;
    #local in2 = 0;
    #local in3 = dimension_size(GVr,3)-1;  
    // real coordinates!!
    #local _gx = (QVr[in1+1][    0][in3  ][0] - QVr[in1  ][in2  ][in3  ][0])/ dd; 
    #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2  ][in3  ][0])/ dd; 
    #local _gz = (QVr[in1  ][    0][in3  ][0] - QVr[in1  ][in2  ][in3-1][0])/ dd; 
    #local gTmp = vlength(<_gx, _gy, _gz>);
    #declare GVr[in1][in2][in3][0] = gTmp;
    #declare GVr[in1][in2][in3][1] = _gx;
    #declare GVr[in1][in2][in3][2] = _gy;
    #declare GVr[in1][in2][in3][3] = _gz;
    #declare GVr[in1][in2][in3][4] = 0; // UNUSED
    
    #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
    #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    DebugMsg(concat("After vertices: Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)    
///////////
    
    DebugMsg("INTERN CLUSTER OF NODES (ie. no vertices - done above - walls and edges).", 1)
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
       #for(in2,1,dimension_size(GVr,2)-2)    // y axis 2nd
          #for(in3,1,dimension_size(GVr,3)-2) // z axis first
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; // double 'dd'
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; // double 'dd'
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) 
                   #local _Gmin = gTmp; 
                   DebugMsg(concat("\n==\nin1=",str(in1,2,0), "; in2=",str(in2,2,0), "; in3=", str(in3,2,0),"\n"), 2)
                   DebugMsg(concat("gx=",str(_gx,14,10), "; gy=",str(_gy,14,10), "; gz=", str(_gz,14,10),"; grd=", str(gTmp,10,5),"\n==\n"), 2)
                #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
             #end
       #end
       DebugMsg(concat("processing: ", str(in1, 4,0)," core gradient X layer \n"), 2)
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    DebugMsg("  Horizontal walls X-Y:", 1)
    // 2a) XY - horizontal walls/lower, Z = const                            
    DebugMsg(concat("processing: X-Y lower wall\n"), 1)
    #local in3 = 0;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
       #for(in2,1,dimension_size(GVr,2)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; // double 'dd'
                #local _gz = (QVr[in1  ][in2  ][    1][0] - QVr[in1  ][in2  ][    0][0])/ dd;
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 2b) XY - horizontal walls/upper, Z = const                            
    DebugMsg(concat("processing: X-Y upper wall\n"), 1)
    #local in3 = dimension_size(GVr,3)-1;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
       #for(in2,1,dimension_size(GVr,2)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; // double 'dd'
                #local _gz = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2  ][in3-1][0])/ dd;
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // vertical walls X-Z:
    // 3a) XZ - vertical walls/origin, Y=const                            
    DebugMsg(concat("processing: X-Z origin wall\n"), 1)
    #local in2 = 0;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
       #for(in3,1,dimension_size(GVr,3)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][    1][in3  ][0] - QVr[in1  ][    0][in3  ][0])/ dd;
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; // double 'dd'
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 3b) XZ - vertical walls/distant, Y=const                                                        
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in2 = dimension_size(GVr,2)-1;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
       #for(in3,1,dimension_size(GVr,3)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/ dd;
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; // double 'dd'
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // vertical walls Y-Z:
    // 3a) YZ - vertical walls/origin, X=const                            
    DebugMsg( concat("processing: X-Z origin wall\n", "Debug Test\n==\n"), 1)
    #local in1 = 0;
    #for(in2,1,dimension_size(GVr,2)-2)       // x axis last
       #for(in3,1,dimension_size(GVr,3)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[    1][in2  ][in3  ][0] - QVr[    0][in2  ][in3  ][0])/ dd;
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; // double 'dd'
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; // double 'dd'
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end                                            
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 3b) YZ - vertical walls/distant, X=const                                                        
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #for(in2,1,dimension_size(GVr,2)-2)       // x axis last
       #for(in3,1,dimension_size(GVr,3)-2)    // y axis 2nd
                // real coordinates!!
                #local _gx = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/ dd;
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; // double 'dd'
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; // double 'dd'
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
       #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4) 12 edges        
    // 4x,1) along X axis Y==0; Z==0;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in2 = 0;
    #local in3 = 0;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][    1][    0][0] - QVr[in1  ][    0][    0][0])/ dd; 
                #local _gz = (QVr[in1  ][    0][    1][0] - QVr[in1  ][    0][    0][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4x,2) along X axis, Y==max; Z==max;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in2 = dimension_size(GVr,2)-1;
    #local in3 = dimension_size(GVr,3)-1;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/ dd; 
                #local _gz = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2  ][in3-1][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4x,3) along X axis, Y==max; Z==0;
    DebugMsg( concat("processing: X-Y distant wall\n"), 1)
    #local in2 = dimension_size(GVr,2)-1;
    #local in3 = 0;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/ dd; 
                #local _gz = (QVr[in1  ][in2  ][    1][0] - QVr[in1  ][in2  ][    0][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4x,4) along X axis, Y==0; Z==max;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in2 = 0;
    #local in3 = dimension_size(GVr,3)-1;
    #for(in1,1,dimension_size(GVr,1)-2)       // x axis last
                // real coordinates!!
                #local _gx = (QVr[in1+1][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/dd2; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2  ][in3  ][0])/ dd; 
                #local _gz = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2  ][in3-1][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4y,1) along Y axis X==0; Z==0;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = 0;
    #local in3 = 0;
    #for(in2,1,dimension_size(GVr,2)-2)       // scan y axis 
                // real coordinates!!
                #local _gx = (QVr[    1][in2  ][    0][0] - QVr[    0][in2  ][    0][0])/ dd; // double 'dd'
                #local _gy = (QVr[    0][in2+1][    0][0] - QVr[    0][in2-1][    0][0])/dd2; 
                #local _gz = (QVr[    0][in2  ][    1][0] - QVr[    0][in2  ][    0][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4y,2) along Y axis, X==max; Z==max;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in3 = dimension_size(GVr,3)-1;
    #for(in2,1,dimension_size(GVr,2)-2)       // scan y axis 
                // real coordinates!!
                #local _gx = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; 
                #local _gz = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2  ][in3-1][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4y,3) along Y axis, X==max; Z==0;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in3 = 0;
    #for(in2,1,dimension_size(GVr,2)-2)       // scan y axis
                // real coordinates!!
                #local _gx = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/dd2; 
                #local _gz = (QVr[in1  ][in2  ][    1][0] - QVr[in1  ][in2  ][    0][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = vTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = vTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4y,4) along Y axis, X==0; Z==max;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = 0;
    #local in3 = dimension_size(GVr,3)-1;
    #for(in2,1,dimension_size(GVr,2)-2)       // scan y axis
                // real coordinates!!
                #local _gx = (QVr[    1][in2  ][in3  ][0] - QVr[    0][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[    0][in2  ][in3  ][0] - QVr[    0][in2-1][in3  ][0])/dd2; 
                #local _gz = (QVr[    0][in2  ][in3  ][0] - QVr[    0][in2  ][in3-1][0])/ dd; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    //===========
    // 4z,1) along Z axis X==0; Y==0;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = 0;
    #local in2 = 0;
    #for(in3,1,dimension_size(GVr,3)-2)       // scan z axis 
                // real coordinates!!
                #local _gx = (QVr[    1][    0][in3  ][0] - QVr[    0][    0][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[    0][    1][in3  ][0] - QVr[    0][    0][in3  ][0])/ dd; 
                #local _gz = (QVr[    0][    0][in3+1][0] - QVr[    0][    0][in3-1][0])/dd2; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4z,2) along Z axis, X==max; Y==max;
    // #debug concat("processing: X-Y distant wall\n")
    #local in1 = dimension_size(GVr,1)-1;
    #local in2 = dimension_size(GVr,2)-1;
    #for(in3,1,dimension_size(GVr,3)-2)       // scan z axis 
                // real coordinates!!
                #local _gx = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1  ][in2-1][in3  ][0])/ dd; 
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4z,3) along Z axis, X==max; Y==0;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = dimension_size(GVr,1)-1;
    #local in2 = 0;
    #for(in3,1,dimension_size(GVr,3)-2)       // scan z axis
                // real coordinates!!
                #local _gx = (QVr[in1  ][in2  ][in3  ][0] - QVr[in1-1][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[in1  ][in2+1][in3  ][0] - QVr[in1  ][in2  ][in3  ][0])/ dd; 
                #local _gz = (QVr[in1  ][in2  ][in3+1][0] - QVr[in1  ][in2  ][in3-1][0])/dd2; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 1)
    
    // 4z,4) along Z axis, X==0; Y==max;
    DebugMsg(concat("processing: X-Y distant wall\n"), 1)
    #local in1 = 0;
    #local in2 = dimension_size(GVr,2)-1;
    #for(in3,1,dimension_size(GVr,3)-2)       // scan z axis
                // real coordinates!!
                #local _gx = (QVr[    1][in2  ][in3  ][0] - QVr[    0][in2  ][in3  ][0])/ dd; // double 'dd'
                #local _gy = (QVr[    0][in2  ][in3  ][0] - QVr[    0][in2-1][in3  ][0])/ dd; 
                #local _gz = (QVr[    0][in2  ][in3+1][0] - QVr[    0][in2  ][in3-1][0])/dd2; 
                #local gTmp = vlength(<_gx, _gy, _gz>);
                #declare GVr[in1][in2][in3][0] = gTmp;
                #declare GVr[in1][in2][in3][1] = _gx;
                #declare GVr[in1][in2][in3][2] = _gy;
                #declare GVr[in1][in2][in3][3] = _gz;
                #declare GVr[in1][in2][in3][4] = 0; // UNUSED
                
                #if (_Gmin > gTmp) #local _Gmin = gTmp; #end
                #if (_Gmax < gTmp) #local _Gmax = gTmp; #end
    #end   
    DebugMsg(concat("Finally: Grad Min=", str(_Gmin,10,5), "; Grad Max=",str(_Gmax,10,5),"\n"), 0)
    
    #if (Debug>0) 
        DebugMsg(concat("Debugging level:",str(Debug,3,0),"\n"), 0)
        #for(in1,0,dimension_size(GVr,1)-1)       // x axis last
           #for(in2,0,dimension_size(GVr,2)-1)    // y axis 2nd
              #for(in3,0,dimension_size(GVr,3)-1) // z axis first
                 #debug concat("in1=",str(in1,2,0), "; in2=",str(in2,2,0), "; in3=", str(in3,2,0),"; grd=", str(GVr[in1][in2][in3][0],10,5),"\n")
              #end
           #end
        #end 
    #end                                           
    #declare GradMin = _Gmin;
    #declare GradMax = _Gmin;
#end // #macro ComputeGridGradient()


  //======================================================//
 //    Drawing facility gradient implementation per cell //
//======================================================//
/****M* Potential.inc/DrawInterpolGrdSrfSmoothed
* PURPOSE
*  Isosurface with smoothed gradient
* SYNOPSIS
*/
#macro DrawInterpolGrdSrfSmoothed(_trsh, _dd) // _trsh - isosurface threshold
/*
* INPUTS
*  _trsh - double - threshold of scalar field;
*  _dd   - 3D cell size (cell is assumed cubic);
* OUTPUTS
*  renders image
* SEE ALSO
*  DrawInterpolGrdSrf (), DrawInterpolGrdSrf1 (), DrawInterpolGrdSrf2 ()
*
******/
DebugMsg(concat("=>dd= ",str(_dd,6,4),"==\n"), 1)   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

    #declare _dd3 = pow(_dd,3);
    #local _CellsFound = 0;
    #for(izn,0,dimension_size(QVr,3)-2)
        DebugMsg(concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ), 1)    
        #for(iyn,0,dimension_size(QVr,2)-2)
            #for(ixn,0,dimension_size(QVr,1)-2)                               
                // do not analyze "empty" cells
                #if (_trsh<min(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                           ) 
                    | _trsh>max(
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing

                #else                                   
                    isosurface{
                      function{VGrid( x, y, z,    // internal conversion from real to internal representation
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    QVr[ixn+1][iyn  ][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn  ][0],   
                                    QVr[ixn+1][iyn+1][izn+1][0],   
                                    QVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    QVr[ixn  ][iyn  ][izn  ][0], 
                                    QVr[ixn  ][iyn+1][izn  ][0],  
                                    QVr[ixn  ][iyn+1][izn+1][0], 
                                    QVr[ixn  ][iyn  ][izn+1][0]
                                    ) 
                      }               
                      contained_by{ 
                            box{
                                 <0, 0, -1>,     // "root" of elementary cell >,
                                 <1, 1,  0>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG ///1.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7  //.1, 1.4, .7
                      open               
                      // texture{_IsoTexture}
                      texture{
                         pigment{ // 
                            function{ clip(log(
                              VGrid( x, y, z,    // internal conversion from real to internal representation
                                    // POVRay's -> Q000, Q100, Q110, Q010,
                                    GVr[ixn+1][iyn  ][izn  ][0],   
                                    GVr[ixn+1][iyn+1][izn  ][0],   
                                    GVr[ixn+1][iyn+1][izn+1][0],   
                                    GVr[ixn+1][iyn  ][izn+1][0],   
                                    
                                    // POVRay's -> Q001, Q101, Q111, Q011, 
                                    GVr[ixn  ][iyn  ][izn  ][0], 
                                    GVr[ixn  ][iyn+1][izn  ][0],  
                                    GVr[ixn  ][iyn+1][izn+1][0], 
                                    GVr[ixn  ][iyn  ][izn+1][0]
                                    ) -_gMin+1), 0, log(_gMax-_gMin+1))/log(_gMax-_gMin+1) }           
                            color_map{_RGBMap}
                         }
                         finish{ scDullMirror }
                      }                                                                               
                      scale _dd
                      translate <QVr[ixn  ][iyn][izn][2], QVr[ixn  ][iyn][izn][3], -QVr[ixn  ][iyn][izn][1]>      // "root" of elementary cell
                    }      
                    #local _CellsFound = _CellsFound+1;
                #end
            #end 
        #end 
        DebugMsg(concat(str(_CellsFound,10,0), " found to date.\n"), 1)    
    #end                                                               
    DebugMsg(concat(str(_CellsFound,10,0), " found in total.\n"), 0)
#end // #macro DrawInterpolGrdSrfSmoothed()

// private macros 
#macro _fmx() (  -pow((xx-rx)/_d,3) +2 *pow((xx-rx)/_d, 2) - (xx-rx)/_d) #end // Q_m_    
#macro _f0x() ( 3*pow((xx-rx)/_d,3) -5 *pow((xx-rx)/_d, 2) + 2)          #end // Q_0_
#macro _f1x() (-3*pow((xx-rx)/_d,3) +4 *pow((xx-rx)/_d, 2) + (xx-rx)/_d) #end // Q_1_
#macro _f2x() (   pow((xx-rx)/_d,3)    -pow((xx-rx)/_d, 2))              #end // Q_2_
                                       
#macro _fmy() (  -pow((yy-ry)/_d,3) +2 *pow((yy-ry)/_d, 2) - (yy-ry)/_d) #end // Q__m    
#macro _f0y() ( 3*pow((yy-ry)/_d,3) -5 *pow((yy-ry)/_d, 2) + 2)          #end // Q__0
#macro _f1y() (-3*pow((yy-ry)/_d,3) +4 *pow((yy-ry)/_d, 2) + (yy-ry)/_d) #end // Q__1
#macro _f2y() (   pow((yy-ry)/_d,3) -   pow((yy-ry)/_d, 2))              #end // Q__2
                                       
#macro _fmz() (  -pow((rz-zz)/_d,3) +2 *pow((rz-zz)/_d, 2) - (rz-zz)/_d) #end // Qm__    
#macro _f0z() ( 3*pow((rz-zz)/_d,3) -5 *pow((rz-zz)/_d, 2) + 2)          #end // Q0__
#macro _f1z() (-3*pow((rz-zz)/_d,3) +4 *pow((rz-zz)/_d, 2) + (rz-zz)/_d) #end // Q1__
#macro _f2z() (   pow((rz-zz)/_d,3) -   pow((rz-zz)/_d, 2))              #end // Q2__

///*   
   //====================================//
  //  key cubic interpolation function  //
 //  internal POVRay coordinates!!!    // 
//====================================//
///// #declare InterCubic = function(
/////                         xx, yy, zz,   // xP, yP, zP: float - internally supplied
/////                         rx, ry, rz,   // float: converted REAL coordinates of lower, left, front corner of elementary cell
/////                         _d            // edge length of elementary cell
/////                         ){            
///// // testowo yy/_d-0.7   //(  -pow((yy-ry)/_d,3)+2*pow((yy-ry)/_d, 2)-(yy-ry)/_d )*Qmmm+ // S_i-1                        
///// // edt: 2018-01-05                                              
///// // indices iterated in "natural" order: (xx-rx), +(yy-ry), -(zz-rz)
///// (
/////     (Q000+Q111)/16
///// /*
/////    _fmy()*( _fmx()*( _fmz()*Qmmm+_f0z()*Q0mm+_f1z()*Q1mm+_f2z()*Q2mm ) + // Q_mm
/////             _f0x()*( _fmz()*Qm0m+_f0z()*Q00m+_f1z()*Q10m+_f2z()*Q20m ) + // Q_0m
/////             _f1x()*( _fmz()*Qm1m+_f0z()*Q01m+_f1z()*Q11m+_f2z()*Q21m ) + // Q_1m
/////             _f2x()*( _fmz()*Qm2m+_f0z()*Q02m+_f1z()*Q12m+_f2z()*Q22m )   // Q_2m
/////           )  + // Q__m + // S_i-1
/////    _f0y()*( _fmx()*( _fmz()*Qmm0+_f0z()*Q0m0+_f1z()*Q1m0+_f2z()*Q2m0 ) + // Q_m0 +
/////             _f0x()*( _fmz()*Qm00+_f0z()*Q000+_f1z()*Q100+_f2z()*Q200 ) + // Q_00   +
/////             _f1x()*( _fmz()*Qm10+_f0z()*Q010+_f1z()*Q110+_f2z()*Q210 ) + // Q_10   +
/////             _f2x()*( _fmz()*Qm20+_f0z()*Q020+_f1z()*Q120+_f2z()*Q220 )   // Q_20
/////           )  + // Q__0 + // S_i
/////    _f1y()*( _fmx()*( _fmz()*Qmm1+_f0z()*Q0m1+_f1z()*Q1m1+_f2z()*Q2m1 ) + // Q_m1 +
/////             _f0x()*( _fmz()*Qm01+_f0z()*Q001+_f1z()*Q101+_f2z()*Q201 ) + // Q_01   +
/////             _f1x()*( _fmz()*Qm11+_f0z()*Q011+_f1z()*Q111+_f2z()*Q211 ) + // Q_11   +
/////             _f2x()*( _fmz()*Qm21+_f0z()*Q021+_f1z()*Q121+_f2z()*Q221 )   // Q_21
/////           )  + // Q__1 + // S_i+1
/////    _f2y()*( _fmx()*( _fmz()*Qmm2+_f0z()*Q0m2+_f1z()*Q1m2+_f2z()*Q2m2 ) + // Q_m2 +
/////             _f0x()*( _fmz()*Qm02+_f0z()*Q002+_f1z()*Q102+_f2z()*Q202 ) + // Q_02   +
/////             _f1x()*( _fmz()*Qm12+_f0z()*Q012+_f1z()*Q112+_f2z()*Q212 ) + // Q_12   +
/////             _f2x()*( _fmz()*Qm22+_f0z()*Q022+_f1z()*Q122+_f2z()*Q222 )   // Q_22
/////           ) // Q__2   // S_i+2
///// )/8.0
///// */
///// )
///// } 
///// //*/       


///*   
   //====================================//
  //  key cubic interpolation function  //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare Sm0 = function(
                        xx, yy, zz,   // xP, yP, zP: float - internally supplied
                        rx, ry, rz,   // float: converted REAL coordinates of lower, left, front corner of elementary cell
                        _d            // edge length of elementary cell
                        ,_Qmmm, _Q0mm, _Q1mm, _Q2mm  // Q_mm
                        ,_Qm0m, _Q00m, _Q10m, _Q20m  // Q_0m
                        ,_Qm1m, _Q01m, _Q11m, _Q21m  // Q_1m
                        ,_Qm2m, _Q02m, _Q12m, _Q22m  // Q_2m
                        ,_Qmm0, _Q0m0, _Q1m0, _Q2m0  // Q_m0 
                        ,_Qm00, _Q000, _Q100, _Q200  // Q_00 
                        ,_Qm10, _Q010, _Q110, _Q210  // Q_10 
                        ,_Qm20, _Q020, _Q120, _Q220  // Q_20
                        ){            
// testowo yy/_d-0.7   //(  -pow((yy-ry)/_d,3)+2*pow((yy-ry)/_d, 2)-(yy-ry)/_d )*Qmmm+ // S_i-1                        
// edt: 2018-01-05                                              
// indices iterated in "natural" order: (xx-rx), +(yy-ry), -(zz-rz)
(
   _fmy()*( _fmx()*( _fmz()*_Qmmm+_f0z()*_Q0mm+_f1z()*_Q1mm+_f2z()*_Q2mm ) + // Q_mm
            _f0x()*( _fmz()*_Qm0m+_f0z()*_Q00m+_f1z()*_Q10m+_f2z()*_Q20m ) + // Q_0m
            _f1x()*( _fmz()*_Qm1m+_f0z()*_Q01m+_f1z()*_Q11m+_f2z()*_Q21m ) + // Q_1m
            _f2x()*( _fmz()*_Qm2m+_f0z()*_Q02m+_f1z()*_Q12m+_f2z()*_Q22m )   // Q_2m
          )  + // Q__m + // _S_i-1       _            _            _
   _f0y()*( _fmx()*( _fmz()*_Qmm0+_f0z()*_Q0m0+_f1z()*_Q1m0+_f2z()*_Q2m0 ) + // Q_m0 +
            _f0x()*( _fmz()*_Qm00+_f0z()*_Q000+_f1z()*_Q100+_f2z()*_Q200 ) + // Q_00   +
            _f1x()*( _fmz()*_Qm10+_f0z()*_Q010+_f1z()*_Q110+_f2z()*_Q210 ) + // Q_10   +
            _f2x()*( _fmz()*_Qm20+_f0z()*_Q020+_f1z()*_Q120+_f2z()*_Q220 )   // Q_20
          ) 
)/8.0
} 
//*/       

///*   
   //====================================//
  //  key cubic interpolation function  //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare S12 = function(
                        xx, yy, zz,   // xP, yP, zP: float - internally supplied
                        rx, ry, rz,   // float: converted REAL coordinates of lower, left, front corner of elementary cell
                        _d            // edge length of elementary cell
                        ,_Qmm1, _Q0m1, _Q1m1, _Q2m1 // Q_m1
                        ,_Qm01, _Q001, _Q101, _Q201 // Q_01
                        ,_Qm11, _Q011, _Q111, _Q211 // Q_11
                        ,_Qm21, _Q021, _Q121, _Q221 // Q_21
                        ,_Qmm2, _Q0m2, _Q1m2, _Q2m2 // Q_m2
                        ,_Qm02, _Q002, _Q102, _Q202 // Q_02
                        ,_Qm12, _Q012, _Q112, _Q212 // Q_12
                        ,_Qm22, _Q022, _Q122, _Q222 // Q_22
                        ){            
// testowo yy/_d-0.7   //(  -pow((yy-ry)/_d,3)+2*pow((yy-ry)/_d, 2)-(yy-ry)/_d )*Qmmm+ // S_i-1                        
// edt: 2018-01-05                                              
// indices iterated in "natural" order: (xx-rx), +(yy-ry), -(zz-rz)
(
   _f1y()*( _fmx()*( _fmz()*_Qmm1+_f0z()*_Q0m1+_f1z()*_Q1m1+_f2z()*_Q2m1 ) + // Q_m1 +
            _f0x()*( _fmz()*_Qm01+_f0z()*_Q001+_f1z()*_Q101+_f2z()*_Q201 ) + // Q_01   +
            _f1x()*( _fmz()*_Qm11+_f0z()*_Q011+_f1z()*_Q111+_f2z()*_Q211 ) + // Q_11   +
            _f2x()*( _fmz()*_Qm21+_f0z()*_Q021+_f1z()*_Q121+_f2z()*_Q221 )   // Q_21
          )  + // Q__1 + // _S_i+1       _            _            _
   _f2y()*( _fmx()*( _fmz()*_Qmm2+_f0z()*_Q0m2+_f1z()*_Q1m2+_f2z()*_Q2m2 ) + // Q_m2 +
            _f0x()*( _fmz()*_Qm02+_f0z()*_Q002+_f1z()*_Q102+_f2z()*_Q202 ) + // Q_02   +
            _f1x()*( _fmz()*_Qm12+_f0z()*_Q012+_f1z()*_Q112+_f2z()*_Q212 ) + // Q_12   +
            _f2x()*( _fmz()*_Qm22+_f0z()*_Q022+_f1z()*_Q122+_f2z()*_Q222 )   // Q_22
          ) // Q__2   // S_i+2
)/8.0
} 
//*/       

   //===========================//
  //   Triple Cubic            //
 //    Drawing facility       //
//===========================//
#macro DrawCubicInterpolSrf(_trsh, _dd) // _trsh - isosurface threshold
#debug concat("=>dd= ",str(_dd,6,4),"==\n")   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#declare _dd3 = pow(_dd,3);
    #for(izn,1,dimension_size(QVr,3)-1)
        #debug concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n")    
        #for(iyn,1,dimension_size(QVr,2)-1)
            #for(ixn,1,dimension_size(QVr,1)-1)                               
                // do not compute "empty" cells
                #if (_trsh<min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else 
                // need to compute pass multinomial coefficients (12pcs)???
                    #declare Qmmm=QVr[ixn-1][iyn-1][izn-1][0];	#declare  Q0mm=QVr[ixn  ][iyn-1][izn-1][0];	#declare  Q1mm=QVr[ixn+1][iyn-1][izn-1][0];	#declare  Q2mm=QVr[ixn+2][iyn-1][izn-1][0];	 //z=izn-1
                    #declare Qm0m=QVr[ixn-1][iyn  ][izn-1][0];	#declare  Q00m=QVr[ixn  ][iyn  ][izn-1][0];	#declare  Q10m=QVr[ixn+1][iyn  ][izn-1][0];	#declare  Q20m=QVr[ixn+2][iyn  ][izn-1][0];	          
                    #declare Qm1m=QVr[ixn-1][iyn+1][izn-1][0];	#declare  Q01m=QVr[ixn  ][iyn+1][izn-1][0];	#declare  Q11m=QVr[ixn+1][iyn+1][izn-1][0];	#declare  Q21m=QVr[ixn+2][iyn+1][izn-1][0];	          
                    #declare Qm2m=QVr[ixn-1][iyn+2][izn-1][0];	#declare  Q02m=QVr[ixn  ][iyn+2][izn-1][0];	#declare  Q12m=QVr[ixn+1][iyn+2][izn-1][0];	#declare  Q22m=QVr[ixn+2][iyn+2][izn-1][0];	          
                    				
                    #declare Qmm0=QVr[ixn-1][iyn-1][izn  ][0];	#declare  Q0m0=QVr[ixn  ][iyn-1][izn  ][0];	#declare  Q1m0=QVr[ixn+1][iyn-1][izn  ][0];	#declare  Q2m0=QVr[ixn+2][iyn-1][izn  ][0];	 //z=izn  
                    #declare Qm00=QVr[ixn-1][iyn  ][izn  ][0];	#declare  Q000=QVr[ixn  ][iyn  ][izn  ][0];	#declare  Q100=QVr[ixn+1][iyn  ][izn  ][0];	#declare  Q200=QVr[ixn+2][iyn  ][izn  ][0];	          
                    #declare Qm10=QVr[ixn-1][iyn+1][izn  ][0];	#declare  Q010=QVr[ixn  ][iyn+1][izn  ][0];	#declare  Q110=QVr[ixn+1][iyn+1][izn  ][0];	#declare  Q210=QVr[ixn+2][iyn+1][izn  ][0];	          
                    #declare Qm20=QVr[ixn-1][iyn+2][izn  ][0];	#declare  Q020=QVr[ixn  ][iyn+2][izn  ][0];	#declare  Q120=QVr[ixn+1][iyn+2][izn  ][0];	#declare  Q220=QVr[ixn+2][iyn+2][izn  ][0];	          
                    				
                    #declare Qmm1=QVr[ixn-1][iyn-1][izn+1][0];	#declare  Q0m1=QVr[ixn  ][iyn-1][izn+1][0];	#declare  Q1m1=QVr[ixn+1][iyn-1][izn+1][0];	#declare  Q2m1=QVr[ixn+2][iyn-1][izn+1][0];	 //z=izn+1
                    #declare Qm01=QVr[ixn-1][iyn  ][izn+1][0];	#declare  Q001=QVr[ixn  ][iyn  ][izn+1][0];	#declare  Q101=QVr[ixn+1][iyn  ][izn+1][0];	#declare  Q201=QVr[ixn+2][iyn  ][izn+1][0];	          
                    #declare Qm11=QVr[ixn-1][iyn+1][izn+1][0];	#declare  Q011=QVr[ixn  ][iyn+1][izn+1][0];	#declare  Q111=QVr[ixn+1][iyn+1][izn+1][0];	#declare  Q211=QVr[ixn+2][iyn+1][izn+1][0];	          
                    #declare Qm21=QVr[ixn-1][iyn+2][izn+1][0];	#declare  Q021=QVr[ixn  ][iyn+2][izn+1][0];	#declare  Q121=QVr[ixn+1][iyn+2][izn+1][0];	#declare  Q221=QVr[ixn+2][iyn+2][izn+1][0];	          
                    				
                    #declare Qmm2=QVr[ixn-1][iyn-1][izn+2][0];	#declare  Q0m2=QVr[ixn  ][iyn-1][izn+2][0];	#declare  Q1m2=QVr[ixn+1][iyn-1][izn+2][0];	#declare  Q2m2=QVr[ixn+2][iyn-1][izn+2][0];	 //z=izn+2
                    #declare Qm02=QVr[ixn-1][iyn  ][izn+2][0];	#declare  Q002=QVr[ixn  ][iyn  ][izn+2][0];	#declare  Q102=QVr[ixn+1][iyn  ][izn+2][0];	#declare  Q202=QVr[ixn+2][iyn  ][izn+2][0];	          
                    #declare Qm12=QVr[ixn-1][iyn+1][izn+2][0];	#declare  Q012=QVr[ixn  ][iyn+1][izn+2][0];	#declare  Q112=QVr[ixn+1][iyn+1][izn+2][0];	#declare  Q212=QVr[ixn+2][iyn+1][izn+2][0];	          
                    #declare Qm22=QVr[ixn-1][iyn+2][izn+2][0];	#declare  Q022=QVr[ixn  ][iyn+2][izn+2][0];	#declare  Q122=QVr[ixn+1][iyn+2][izn+2][0];	#declare  Q222=QVr[ixn+2][iyn+2][izn+2][0];	
// testing..
#if (Debug>0)
    
    #local sx =  (QVr[ixn][iyn][izn][2]+QVr[ixn  ][iyn+1][izn  ][2])/2;
    #local sy =  (QVr[ixn][iyn][izn][3]+QVr[ixn  ][iyn  ][izn+1][3])/2;
    #local sz = -(QVr[ixn][iyn][izn][1]+QVr[ixn+1][iyn  ][izn  ][1])/2;
    #local CV = Sm0(sz,sx,sy,
                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                    _dd
                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                    ,Qm00, Q000, Q100, Q200  // Q00 
                    ,Qm10, Q010, Q110, Q210  // Q10 
                    ,Qm20, Q020, Q120, Q220  // Q20
                    )
              + S12(sz,sx,sy,
                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                   _dd
                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                    ,Qm01, Q001, Q101, Q201 // Q01
                    ,Qm11, Q011, Q111, Q211 // Q11
                    ,Qm21, Q021, Q121, Q221 // Q21
                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                    ,Qm02, Q002, Q102, Q202 // Q02
                    ,Qm12, Q012, Q112, Q212 // Q12
                    ,Qm22, Q022, Q122, Q222 // Q22
                    );
    #debug concat("at <", vstr(3, <-sz,sx,sy>,"; ",4,2), ">\tintpd V(r)= ", str(CV, 15,7), ", but cmptd  V(r)= ", str((Q000+Q111)/2, 15, 7), "\n" )                        
    sphere{
       < sx, sy, sz>, _dd/4
    //                       texture{pigment{color rgb <1,abs(QVr[ixn][iyn][izn][3]),CV>} finish{emission .1}}
       texture{pigment{color rgb abs(CV) } finish{emission .1}}
    }                 
#end // Debug
                    
                    isosurface{
                      function{
                                Sm0(x,y,z,
                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                                    _dd
                                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                                    ,Qm00, Q000, Q100, Q200  // Q00 
                                    ,Qm10, Q010, Q110, Q210  // Q10 
                                    ,Qm20, Q020, Q120, Q220  // Q20
                                    )
                              + S12(x, y, z,
                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                                   _dd
                                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                                    ,Qm01, Q001, Q101, Q201 // Q01
                                    ,Qm11, Q011, Q111, Q211 // Q11
                                    ,Qm21, Q021, Q121, Q221 // Q21
                                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                                    ,Qm02, Q002, Q102, Q202 // Q02
                                    ,Qm12, Q012, Q112, Q212 // Q12
                                    ,Qm22, Q022, Q122, Q222 // Q22
                                    )/8.0;
                      }               
                      contained_by{ 
                            box{
                                 <QVr[  ixn][  iyn][  izn][2], QVr[  ixn][  iyn][  izn][3], -QVr[  ixn][  iyn][  izn][1]>,     // "root" of elementary cell >,
                                 <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn+1][1]>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient  _MaxG // 10  
                      evaluate _MaxG*.7, sqrt(1/.7), .7 //.1, 1.4, .7
                      open               
                      texture{_IsoTexture}
                    }
                #end
            #end // ixn
        #end // iyn 
   #end // izn
#end


// 2 - private macros in primitive cell
#macro _cfmx() (  -pow(xx,3) +2 *pow(xx, 2) - xx)   #end // Q_m_    
#macro _cf0x() ( 3*pow(xx,3) -5 *pow(xx, 2) + 2)    #end // Q_0_
#macro _cf1x() (-3*pow(xx,3) +4 *pow(xx, 2) + xx)   #end // Q_1_
#macro _cf2x() (   pow(xx,3)    -pow(xx, 2))        #end // Q_2_
                                                    
#macro _cfmy() (  -pow(yy,3) +2 *pow(yy, 2) - yy)   #end // Q__m    
#macro _cf0y() ( 3*pow(yy,3) -5 *pow(yy, 2) + 2)    #end // Q__0
#macro _cf1y() (-3*pow(yy,3) +4 *pow(yy, 2) + yy)   #end // Q__1
#macro _cf2y() (   pow(yy,3) -   pow(yy, 2))        #end // Q__2
                                                    
#macro _cfmz() (  -pow((-zz),3) +2 *pow((-zz), 2) + zz) #end // Qm__    
#macro _cf0z() ( 3*pow((-zz),3) -5 *pow((-zz), 2) + 2)  #end // Q0__
#macro _cf1z() (-3*pow((-zz),3) +4 *pow((-zz), 2) - zz) #end // Q1__
#macro _cf2z() (   pow((-zz),3) -   pow((-zz), 2))      #end // Q2__
//=============================================================================//

///* Primitive cell  
    //====================================//
   //  key cubic interpolation function  //
  //  internal POVRay coordinates!!!    // 
 // primitive cell attempt             //
//====================================//
#declare cSm0 = function(
                        xx, yy, zz   // xP, yP, zP: float - internally supplied
                        ,_Qmmm, _Q0mm, _Q1mm, _Q2mm  // Q_mm
                        ,_Qm0m, _Q00m, _Q10m, _Q20m  // Q_0m
                        ,_Qm1m, _Q01m, _Q11m, _Q21m  // Q_1m
                        ,_Qm2m, _Q02m, _Q12m, _Q22m  // Q_2m
                        ,_Qmm0, _Q0m0, _Q1m0, _Q2m0  // Q_m0 
                        ,_Qm00, _Q000, _Q100, _Q200  // Q_00 
                        ,_Qm10, _Q010, _Q110, _Q210  // Q_10 
                        ,_Qm20, _Q020, _Q120, _Q220  // Q_20
                        ){            
(
   _cfmy()*( _cfmx()*( _cfmz()*_Qmmm+_cf0z()*_Q0mm+_cf1z()*_Q1mm+_cf2z()*_Q2mm ) + // Q_mm
             _cf0x()*( _cfmz()*_Qm0m+_cf0z()*_Q00m+_cf1z()*_Q10m+_cf2z()*_Q20m ) + // Q_0m
             _cf1x()*( _cfmz()*_Qm1m+_cf0z()*_Q01m+_cf1z()*_Q11m+_cf2z()*_Q21m ) + // Q_1m
             _cf2x()*( _cfmz()*_Qm2m+_cf0z()*_Q02m+_cf1z()*_Q12m+_cf2z()*_Q22m )   // Q_2m
           ) + // Q__m + // _S_i-1       _            _            _
   _cf0y()*( _cfmx()*( _cfmz()*_Qmm0+_cf0z()*_Q0m0+_cf1z()*_Q1m0+_cf2z()*_Q2m0 ) + // Q_m0 +
             _cf0x()*( _cfmz()*_Qm00+_cf0z()*_Q000+_cf1z()*_Q100+_cf2z()*_Q200 ) + // Q_00   +
             _cf1x()*( _cfmz()*_Qm10+_cf0z()*_Q010+_cf1z()*_Q110+_cf2z()*_Q210 ) + // Q_10   +
             _cf2x()*( _cfmz()*_Qm20+_cf0z()*_Q020+_cf1z()*_Q120+_cf2z()*_Q220 )   // Q_20
           ) 
)  

} 
//*/       

///*   Primitive cell /2
    //====================================//
   //  key cubic interpolation function  //
  //  internal POVRay coordinates!!!    // 
 // primitive cell attempt             //
//====================================//
#declare cS12 = function(
                        xx, yy, zz   // xP, yP, zP: float - internally supplied
                        ,_Qmm1, _Q0m1, _Q1m1, _Q2m1 // Q_m1
                        ,_Qm01, _Q001, _Q101, _Q201 // Q_01
                        ,_Qm11, _Q011, _Q111, _Q211 // Q_11
                        ,_Qm21, _Q021, _Q121, _Q221 // Q_21
                        ,_Qmm2, _Q0m2, _Q1m2, _Q2m2 // Q_m2
                        ,_Qm02, _Q002, _Q102, _Q202 // Q_02
                        ,_Qm12, _Q012, _Q112, _Q212 // Q_12
                        ,_Qm22, _Q022, _Q122, _Q222 // Q_22
                        ){            
// indices iterated in "natural" order: (xx-rx), +(yy-ry), -(zz-rz)
(
   _cf1y()*( _cfmx()*( _cfmz()*_Qmm1+_cf0z()*_Q0m1+_cf1z()*_Q1m1+_cf2z()*_Q2m1 ) + // Q_m1 +
             _cf0x()*( _cfmz()*_Qm01+_cf0z()*_Q001+_cf1z()*_Q101+_cf2z()*_Q201 ) + // Q_01   +
             _cf1x()*( _cfmz()*_Qm11+_cf0z()*_Q011+_cf1z()*_Q111+_cf2z()*_Q211 ) + // Q_11   +
             _cf2x()*( _cfmz()*_Qm21+_cf0z()*_Q021+_cf1z()*_Q121+_cf2z()*_Q221 )   // Q_21
           ) + // Q__1 + // _S_i+1       _            _            _
   _cf2y()*( _cfmx()*( _cfmz()*_Qmm2+_cf0z()*_Q0m2+_cf1z()*_Q1m2+_cf2z()*_Q2m2 ) + // Q_m2 +
             _cf0x()*( _cfmz()*_Qm02+_cf0z()*_Q002+_cf1z()*_Q102+_cf2z()*_Q202 ) + // Q_02   +
             _cf1x()*( _cfmz()*_Qm12+_cf0z()*_Q012+_cf1z()*_Q112+_cf2z()*_Q212 ) + // Q_12   +
             _cf2x()*( _cfmz()*_Qm22+_cf0z()*_Q022+_cf1z()*_Q122+_cf2z()*_Q222 )   // Q_22
           ) // Q__2   // S_i+2
)
} 
//*/       

   //===========================//
  //   Triple Cubic            //
 //    Drawing facility       //
//===========================//
// TriCubic
/****M* Potential.inc/Draw01CubicInterpolSrf
* PURPOSE
*  Computes isosurface usin TriCubic interpolation. Setting variable "Debug" to more than zero produces diagnostic output
* SYNOPSIS
*/
#macro Draw01CubicInterpolSrf (_trsh, _dd) // _trsh - isosurface threshold
/*
* INPUTS
*  _trsh - double - threshold value of isosurface
*  _dd   - double - edge length of elementary cell
* QVr[]  - garray - implicit attribute, nodes of computation frame
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf ()
*
******/
#debug concat("=>dd= ",str(_dd,6,4),"==\n")   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#declare _dd3 = pow(_dd,3);
    #for(izn,1,dimension_size(QVr,3)-3)
        #debug concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n")    
        #for(iyn,1,dimension_size(QVr,2)-3)
            #for(ixn,1,dimension_size(QVr,1)-3)                               
                // do not compute "empty" cells
                #local _tMin =  min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                               ); 
                #local _tMax = max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                               );                                     
                #if ((_trsh <_tMin-2*(_tMax-_tMin) | _trsh>_tMax+2*(_tMax-_tMin) | (QVr[ixn][iyn][izn][4]))) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else 
                // need to compute pass multinomial coefficients (12pcs)???
                    #declare Qmmm=QVr[ixn-1][iyn-1][izn-1][0];	#declare  Q0mm=QVr[ixn  ][iyn-1][izn-1][0];	#declare  Q1mm=QVr[ixn+1][iyn-1][izn-1][0];	#declare  Q2mm=QVr[ixn+2][iyn-1][izn-1][0];	 //z=izn-1
                    #declare Qm0m=QVr[ixn-1][iyn  ][izn-1][0];	#declare  Q00m=QVr[ixn  ][iyn  ][izn-1][0];	#declare  Q10m=QVr[ixn+1][iyn  ][izn-1][0];	#declare  Q20m=QVr[ixn+2][iyn  ][izn-1][0];	          
                    #declare Qm1m=QVr[ixn-1][iyn+1][izn-1][0];	#declare  Q01m=QVr[ixn  ][iyn+1][izn-1][0];	#declare  Q11m=QVr[ixn+1][iyn+1][izn-1][0];	#declare  Q21m=QVr[ixn+2][iyn+1][izn-1][0];	          
                    #declare Qm2m=QVr[ixn-1][iyn+2][izn-1][0];	#declare  Q02m=QVr[ixn  ][iyn+2][izn-1][0];	#declare  Q12m=QVr[ixn+1][iyn+2][izn-1][0];	#declare  Q22m=QVr[ixn+2][iyn+2][izn-1][0];	          
                    				
                    #declare Qmm0=QVr[ixn-1][iyn-1][izn  ][0];	#declare  Q0m0=QVr[ixn  ][iyn-1][izn  ][0];	#declare  Q1m0=QVr[ixn+1][iyn-1][izn  ][0];	#declare  Q2m0=QVr[ixn+2][iyn-1][izn  ][0];	 //z=izn  
                    #declare Qm00=QVr[ixn-1][iyn  ][izn  ][0];	#declare  Q000=QVr[ixn  ][iyn  ][izn  ][0];	#declare  Q100=QVr[ixn+1][iyn  ][izn  ][0];	#declare  Q200=QVr[ixn+2][iyn  ][izn  ][0];	          
                    #declare Qm10=QVr[ixn-1][iyn+1][izn  ][0];	#declare  Q010=QVr[ixn  ][iyn+1][izn  ][0];	#declare  Q110=QVr[ixn+1][iyn+1][izn  ][0];	#declare  Q210=QVr[ixn+2][iyn+1][izn  ][0];	          
                    #declare Qm20=QVr[ixn-1][iyn+2][izn  ][0];	#declare  Q020=QVr[ixn  ][iyn+2][izn  ][0];	#declare  Q120=QVr[ixn+1][iyn+2][izn  ][0];	#declare  Q220=QVr[ixn+2][iyn+2][izn  ][0];	          
                    				
                    #declare Qmm1=QVr[ixn-1][iyn-1][izn+1][0];	#declare  Q0m1=QVr[ixn  ][iyn-1][izn+1][0];	#declare  Q1m1=QVr[ixn+1][iyn-1][izn+1][0];	#declare  Q2m1=QVr[ixn+2][iyn-1][izn+1][0];	 //z=izn+1
                    #declare Qm01=QVr[ixn-1][iyn  ][izn+1][0];	#declare  Q001=QVr[ixn  ][iyn  ][izn+1][0];	#declare  Q101=QVr[ixn+1][iyn  ][izn+1][0];	#declare  Q201=QVr[ixn+2][iyn  ][izn+1][0];	          
                    #declare Qm11=QVr[ixn-1][iyn+1][izn+1][0];	#declare  Q011=QVr[ixn  ][iyn+1][izn+1][0];	#declare  Q111=QVr[ixn+1][iyn+1][izn+1][0];	#declare  Q211=QVr[ixn+2][iyn+1][izn+1][0];	          
                    #declare Qm21=QVr[ixn-1][iyn+2][izn+1][0];	#declare  Q021=QVr[ixn  ][iyn+2][izn+1][0];	#declare  Q121=QVr[ixn+1][iyn+2][izn+1][0];	#declare  Q221=QVr[ixn+2][iyn+2][izn+1][0];	          
                    				
                    #declare Qmm2=QVr[ixn-1][iyn-1][izn+2][0];	#declare  Q0m2=QVr[ixn  ][iyn-1][izn+2][0];	#declare  Q1m2=QVr[ixn+1][iyn-1][izn+2][0];	#declare  Q2m2=QVr[ixn+2][iyn-1][izn+2][0];	 //z=izn+2
                    #declare Qm02=QVr[ixn-1][iyn  ][izn+2][0];	#declare  Q002=QVr[ixn  ][iyn  ][izn+2][0];	#declare  Q102=QVr[ixn+1][iyn  ][izn+2][0];	#declare  Q202=QVr[ixn+2][iyn  ][izn+2][0];	          
                    #declare Qm12=QVr[ixn-1][iyn+1][izn+2][0];	#declare  Q012=QVr[ixn  ][iyn+1][izn+2][0];	#declare  Q112=QVr[ixn+1][iyn+1][izn+2][0];	#declare  Q212=QVr[ixn+2][iyn+1][izn+2][0];	          
                    #declare Qm22=QVr[ixn-1][iyn+2][izn+2][0];	#declare  Q022=QVr[ixn  ][iyn+2][izn+2][0];	#declare  Q122=QVr[ixn+1][iyn+2][izn+2][0];	#declare  Q222=QVr[ixn+2][iyn+2][izn+2][0];	
// testing..
#if (Debug>0)
    
    #local CV =( cSm0(.5, .5, .5
                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                    ,Qm00, Q000, Q100, Q200  // Q00 
                    ,Qm10, Q010, Q110, Q210  // Q10 
                    ,Qm20, Q020, Q120, Q220  // Q20
                    )
              +  cS12(.5, .5, .5
                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                    ,Qm01, Q001, Q101, Q201 // Q01
                    ,Qm11, Q011, Q111, Q211 // Q11
                    ,Qm21, Q021, Q121, Q221 // Q21
                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                    ,Qm02, Q002, Q102, Q202 // Q02
                    ,Qm12, Q012, Q112, Q212 // Q12
                    ,Qm22, Q022, Q122, Q222 // Q22
                    ))/8;
    #debug concat("at <", vstr(3, <-sz,sx,sy>,"; ",4,2), ">\tintpd V(r)= ", str(CV, 15,7), ", but cmptd  V(r)= ", str((Q000+Q111)/2, 15, 7), "\n" )                        
    sphere{
       < sx, sy, sz>, _dd/4
    //                       texture{pigment{color rgb <1,abs(QVr[ixn][iyn][izn][3]),CV>} finish{emission .1}}
       texture{pigment{color rgb abs(CV) } finish{emission .1}}
    }                 
#end // Debug
            isosurface{
                      function{
                                (cSm0(x, y, z
                                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                                    ,Qm00, Q000, Q100, Q200  // Q00 
                                    ,Qm10, Q010, Q110, Q210  // Q10 
                                    ,Qm20, Q020, Q120, Q220  // Q20
                                    ) + 
                                 cS12(x, y, z
                                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                                    ,Qm01, Q001, Q101, Q201 // Q01
                                    ,Qm11, Q011, Q111, Q211 // Q11
                                    ,Qm21, Q021, Q121, Q221 // Q21
                                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                                    ,Qm02, Q002, Q102, Q202 // Q02
                                    ,Qm12, Q012, Q112, Q212 // Q12
                                    ,Qm22, Q022, Q122, Q222 // Q22
                                    ))/8.0
                      }               
                      contained_by{ 
                            box{
                                 <0, 0,  -1>,     // "root" of elementary cell >,
                                 <1, 1,   0>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG // 10.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7 //.1, 1.4, .7
                      open               
                      scale _dd
                      translate<QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1]> // float real "root" of elementary cell
                      texture{_IsoTexture}
                    }  
                #end
            #end // ixn
        #end // iyn 
   #end // izn
#end // Draw01CubicInterpolSrf Cubic based in basic cell
//===============================================================:)
//===============================================================:)
//===============================================================:)
//===============================================================:)

// set of private macros - parametrized Catmul-Rom formulation
// _TanTau - global tangent parameter: 0.5-> uniform Catmul-Rom, but values "_t>0" are all possible
// coefficients are computed at start of the program
#macro _crfmx() (         -_TanTau*pow(xx,3) +    2*_TanTau* pow(xx, 2) - _TanTau*xx)     #end // Q_m_    
#macro _crf0x() (      (2-_TanTau)*pow(xx,3) +  (_TanTau-3)* pow(xx, 2) + 1)              #end // Q_0_
#macro _crf1x() (      (_TanTau-2)*pow(xx,3) +(3-2*_TanTau)* pow(xx, 2) + _TanTau*xx)     #end // Q_1_
#macro _crf2x() (          _TanTau*pow(xx,3)    -   _TanTau* pow(xx, 2))                  #end // Q_2_
                                                                                       
#macro _crfmy() (         -_TanTau*pow(yy,3) +     2*_TanTau*pow(yy, 2) - _TanTau*yy)     #end // Q__m    
#macro _crf0y() (      (2-_TanTau)*pow(yy,3) +  (_TanTau-3) *pow(yy, 2) + 1)              #end // Q__0
#macro _crf1y() (      (_TanTau-2)*pow(yy,3) +(3-2*_TanTau) *pow(yy, 2) + _TanTau*yy)     #end // Q__1
#macro _crf2y() (          _TanTau*pow(yy,3) -       _TanTau*pow(yy, 2))                  #end // Q__2
                                                     
#macro _crfmz() (      -_TanTau*pow((-zz),3) +     2*_TanTau*pow((-zz), 2) + _TanTau*zz)  #end // Qm__    
#macro _crf0z() (   (2-_TanTau)*pow((-zz),3) +  (_TanTau-3) *pow((-zz), 2) + 1)           #end // Q0__
#macro _crf1z() (   (_TanTau-2)*pow((-zz),3) + (3-2*_TanTau)*pow((-zz), 2) - _TanTau*zz)  #end // Q1__
#macro _crf2z() (       _TanTau*pow((-zz),3) -       _TanTau*pow((-zz), 2))               #end // Q2__
//=============================================================================//

///* Extended cell - Lower part
    //====================================//
   //  key cubic interpolation function  //
  //  internal POVRay coordinates!!!    // 
 // primitive cell attempt             //
//====================================//
#declare crSm0 = function(
                        xx, yy, zz   // xP, yP, zP: float - internally supplied    
                        ,_Qmmm, _Q0mm, _Q1mm, _Q2mm  // Q_mm
                        ,_Qm0m, _Q00m, _Q10m, _Q20m  // Q_0m
                        ,_Qm1m, _Q01m, _Q11m, _Q21m  // Q_1m
                        ,_Qm2m, _Q02m, _Q12m, _Q22m  // Q_2m
                        ,_Qmm0, _Q0m0, _Q1m0, _Q2m0  // Q_m0 
                        ,_Qm00, _Q000, _Q100, _Q200  // Q_00 
                        ,_Qm10, _Q010, _Q110, _Q210  // Q_10 
                        ,_Qm20, _Q020, _Q120, _Q220  // Q_20
                        ){            
(
   _crfmy()*( _crfmx()*( _crfmz()*_Qmmm+_crf0z()*_Q0mm+_crf1z()*_Q1mm+_crf2z()*_Q2mm ) + // Q_mm
              _crf0x()*( _crfmz()*_Qm0m+_crf0z()*_Q00m+_crf1z()*_Q10m+_crf2z()*_Q20m ) + // Q_0m
              _crf1x()*( _crfmz()*_Qm1m+_crf0z()*_Q01m+_crf1z()*_Q11m+_crf2z()*_Q21m ) + // Q_1m
              _crf2x()*( _crfmz()*_Qm2m+_crf0z()*_Q02m+_crf1z()*_Q12m+_crf2z()*_Q22m )   // Q_2m
           ) + // Q__m + // _S_i-1       _            _            _
   _crf0y()*( _crfmx()*( _crfmz()*_Qmm0+_crf0z()*_Q0m0+_crf1z()*_Q1m0+_crf2z()*_Q2m0 ) + // Q_m0 +
              _crf0x()*( _crfmz()*_Qm00+_crf0z()*_Q000+_crf1z()*_Q100+_crf2z()*_Q200 ) + // Q_00   +
              _crf1x()*( _crfmz()*_Qm10+_crf0z()*_Q010+_crf1z()*_Q110+_crf2z()*_Q210 ) + // Q_10   +
              _crf2x()*( _crfmz()*_Qm20+_crf0z()*_Q020+_crf1z()*_Q120+_crf2z()*_Q220 )   // Q_20
           ) 
)  

} 
//*/       

///*   Extended cell - upper part
    //====================================//
   //  key Catmul-Rom function           //
  //  internal POVRay coordinates!!!    // 
 //  primitive cell attempt            //
//====================================//
#declare crS12 = function(
                        xx, yy, zz   // xP, yP, zP: float - internally supplied
                        ,_Qmm1, _Q0m1, _Q1m1, _Q2m1 // Q_m1
                        ,_Qm01, _Q001, _Q101, _Q201 // Q_01
                        ,_Qm11, _Q011, _Q111, _Q211 // Q_11
                        ,_Qm21, _Q021, _Q121, _Q221 // Q_21
                        ,_Qmm2, _Q0m2, _Q1m2, _Q2m2 // Q_m2
                        ,_Qm02, _Q002, _Q102, _Q202 // Q_02
                        ,_Qm12, _Q012, _Q112, _Q212 // Q_12
                        ,_Qm22, _Q022, _Q122, _Q222 // Q_22
                        ){            
// indices iterated in "natural" order: (xx-rx), +(yy-ry), -(zz-rz)
(
   _crf1y()*( _crfmx()*( _crfmz()*_Qmm1+_crf0z()*_Q0m1+_crf1z()*_Q1m1+_crf2z()*_Q2m1 ) + // Q_m1 +
              _crf0x()*( _crfmz()*_Qm01+_crf0z()*_Q001+_crf1z()*_Q101+_crf2z()*_Q201 ) + // Q_01   +
              _crf1x()*( _crfmz()*_Qm11+_crf0z()*_Q011+_crf1z()*_Q111+_crf2z()*_Q211 ) + // Q_11   +
              _crf2x()*( _crfmz()*_Qm21+_crf0z()*_Q021+_crf1z()*_Q121+_crf2z()*_Q221 )   // Q_21
           ) + // Q__1 + // _S_i+1       _            _            _
   _crf2y()*( _crfmx()*( _crfmz()*_Qmm2+_crf0z()*_Q0m2+_crf1z()*_Q1m2+_crf2z()*_Q2m2 ) + // Q_m2 +
              _crf0x()*( _crfmz()*_Qm02+_crf0z()*_Q002+_crf1z()*_Q102+_crf2z()*_Q202 ) + // Q_02   +
              _crf1x()*( _crfmz()*_Qm12+_crf0z()*_Q012+_crf1z()*_Q112+_crf2z()*_Q212 ) + // Q_12   +
              _crf2x()*( _crfmz()*_Qm22+_crf0z()*_Q022+_crf1z()*_Q122+_crf2z()*_Q222 )   // Q_22
           ) // Q__2   // S_i+2
)
} 
//*/       

   //===========================//
  //   Triple Cubic            //
 //    Drawing facility       //
//===========================//
// TriCubic
/****M* Potential.inc/DrawCatRomSrf 
* PURPOSE
*  Computes isosurface usin TriCubic interpolation. Setting variable "Debug" to more than zero produces diagnostic output
* SYNOPSIS
*/
#macro DrawCatRomSrf (_trsh, _dd) // _trsh - isosurface threshold
/*
* INPUTS
*  _trsh - double - threshold value of isosurface
*  _dd   - double - edge length of elementary cell
* QVr[]  - garray - implicit attribute, nodes of computation frame
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf ()
*
******/
#debug concat("=>dd= ",str(_dd,6,4),"==\n")   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#declare _dd3 = pow(_dd,3);
    #for(izn,1,dimension_size(QVr,3)-3)
        #debug concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n")    
        #for(iyn,1,dimension_size(QVr,2)-3)
            #for(ixn,1,dimension_size(QVr,1)-3)                               
                // do not compute "empty" cells
                #local _tMin =  min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                               ); 
                #local _tMax = max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                               );                                     
                #if ((_trsh <_tMin-_exx*(_tMax-_tMin) | _trsh>_tMax+_exx*(_tMax-_tMin) | (QVr[ixn][iyn][izn][4]))  ) // do nothing
//                #if ((_trsh <_tMin | _trsh>_tMax | (QVr[ixn][iyn][izn][4]))) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else 
                // need to compute pass multinomial coefficients (12pcs)???
                    #declare Qmmm=QVr[ixn-1][iyn-1][izn-1][0];	#declare  Q0mm=QVr[ixn  ][iyn-1][izn-1][0];	#declare  Q1mm=QVr[ixn+1][iyn-1][izn-1][0];	#declare  Q2mm=QVr[ixn+2][iyn-1][izn-1][0];	 //z=izn-1
                    #declare Qm0m=QVr[ixn-1][iyn  ][izn-1][0];	#declare  Q00m=QVr[ixn  ][iyn  ][izn-1][0];	#declare  Q10m=QVr[ixn+1][iyn  ][izn-1][0];	#declare  Q20m=QVr[ixn+2][iyn  ][izn-1][0];	          
                    #declare Qm1m=QVr[ixn-1][iyn+1][izn-1][0];	#declare  Q01m=QVr[ixn  ][iyn+1][izn-1][0];	#declare  Q11m=QVr[ixn+1][iyn+1][izn-1][0];	#declare  Q21m=QVr[ixn+2][iyn+1][izn-1][0];	          
                    #declare Qm2m=QVr[ixn-1][iyn+2][izn-1][0];	#declare  Q02m=QVr[ixn  ][iyn+2][izn-1][0];	#declare  Q12m=QVr[ixn+1][iyn+2][izn-1][0];	#declare  Q22m=QVr[ixn+2][iyn+2][izn-1][0];	          
                    				
                    #declare Qmm0=QVr[ixn-1][iyn-1][izn  ][0];	#declare  Q0m0=QVr[ixn  ][iyn-1][izn  ][0];	#declare  Q1m0=QVr[ixn+1][iyn-1][izn  ][0];	#declare  Q2m0=QVr[ixn+2][iyn-1][izn  ][0];	 //z=izn  
                    #declare Qm00=QVr[ixn-1][iyn  ][izn  ][0];	#declare  Q000=QVr[ixn  ][iyn  ][izn  ][0];	#declare  Q100=QVr[ixn+1][iyn  ][izn  ][0];	#declare  Q200=QVr[ixn+2][iyn  ][izn  ][0];	          
                    #declare Qm10=QVr[ixn-1][iyn+1][izn  ][0];	#declare  Q010=QVr[ixn  ][iyn+1][izn  ][0];	#declare  Q110=QVr[ixn+1][iyn+1][izn  ][0];	#declare  Q210=QVr[ixn+2][iyn+1][izn  ][0];	          
                    #declare Qm20=QVr[ixn-1][iyn+2][izn  ][0];	#declare  Q020=QVr[ixn  ][iyn+2][izn  ][0];	#declare  Q120=QVr[ixn+1][iyn+2][izn  ][0];	#declare  Q220=QVr[ixn+2][iyn+2][izn  ][0];	          
                    				
                    #declare Qmm1=QVr[ixn-1][iyn-1][izn+1][0];	#declare  Q0m1=QVr[ixn  ][iyn-1][izn+1][0];	#declare  Q1m1=QVr[ixn+1][iyn-1][izn+1][0];	#declare  Q2m1=QVr[ixn+2][iyn-1][izn+1][0];	 //z=izn+1
                    #declare Qm01=QVr[ixn-1][iyn  ][izn+1][0];	#declare  Q001=QVr[ixn  ][iyn  ][izn+1][0];	#declare  Q101=QVr[ixn+1][iyn  ][izn+1][0];	#declare  Q201=QVr[ixn+2][iyn  ][izn+1][0];	          
                    #declare Qm11=QVr[ixn-1][iyn+1][izn+1][0];	#declare  Q011=QVr[ixn  ][iyn+1][izn+1][0];	#declare  Q111=QVr[ixn+1][iyn+1][izn+1][0];	#declare  Q211=QVr[ixn+2][iyn+1][izn+1][0];	          
                    #declare Qm21=QVr[ixn-1][iyn+2][izn+1][0];	#declare  Q021=QVr[ixn  ][iyn+2][izn+1][0];	#declare  Q121=QVr[ixn+1][iyn+2][izn+1][0];	#declare  Q221=QVr[ixn+2][iyn+2][izn+1][0];	          
                    				
                    #declare Qmm2=QVr[ixn-1][iyn-1][izn+2][0];	#declare  Q0m2=QVr[ixn  ][iyn-1][izn+2][0];	#declare  Q1m2=QVr[ixn+1][iyn-1][izn+2][0];	#declare  Q2m2=QVr[ixn+2][iyn-1][izn+2][0];	 //z=izn+2
                    #declare Qm02=QVr[ixn-1][iyn  ][izn+2][0];	#declare  Q002=QVr[ixn  ][iyn  ][izn+2][0];	#declare  Q102=QVr[ixn+1][iyn  ][izn+2][0];	#declare  Q202=QVr[ixn+2][iyn  ][izn+2][0];	          
                    #declare Qm12=QVr[ixn-1][iyn+1][izn+2][0];	#declare  Q012=QVr[ixn  ][iyn+1][izn+2][0];	#declare  Q112=QVr[ixn+1][iyn+1][izn+2][0];	#declare  Q212=QVr[ixn+2][iyn+1][izn+2][0];	          
                    #declare Qm22=QVr[ixn-1][iyn+2][izn+2][0];	#declare  Q022=QVr[ixn  ][iyn+2][izn+2][0];	#declare  Q122=QVr[ixn+1][iyn+2][izn+2][0];	#declare  Q222=QVr[ixn+2][iyn+2][izn+2][0];	
// testing..
#if (Debug>0)
    
    #local CV =( crSm0(.5, .5, .5
                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                    ,Qm00, Q000, Q100, Q200  // Q00 
                    ,Qm10, Q010, Q110, Q210  // Q10 
                    ,Qm20, Q020, Q120, Q220  // Q20
                    )
              +  crS12(.5, .5, .5
                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                    ,Qm01, Q001, Q101, Q201 // Q01
                    ,Qm11, Q011, Q111, Q211 // Q11
                    ,Qm21, Q021, Q121, Q221 // Q21
                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                    ,Qm02, Q002, Q102, Q202 // Q02
                    ,Qm12, Q012, Q112, Q212 // Q12
                    ,Qm22, Q022, Q122, Q222 // Q22
                    ));
    #debug concat("at <", vstr(3, <-sz,sx,sy>,"; ",4,2), ">\tintpd V(r)= ", str(CV, 15,7), ", but cmptd  V(r)= ", str((Q000+Q111)/2, 15, 7), "\n" )                        
    sphere{
       < sx, sy, sz>, _dd/4
    // texture{pigment{color rgb <1,abs(QVr[ixn][iyn][izn][3]),CV>} finish{emission .1}}
       texture{pigment{color rgb abs(CV) } finish{emission .1}}
    }                 
#end // Debug
            isosurface{
                      function{
                                (crSm0(x, y, z
                                    ,Qmmm, Q0mm, Q1mm, Q2mm  // Qmm
                                    ,Qm0m, Q00m, Q10m, Q20m  // Q0m
                                    ,Qm1m, Q01m, Q11m, Q21m  // Q1m
                                    ,Qm2m, Q02m, Q12m, Q22m  // Q2m
                                    ,Qmm0, Q0m0, Q1m0, Q2m0  // Qm0 
                                    ,Qm00, Q000, Q100, Q200  // Q00 
                                    ,Qm10, Q010, Q110, Q210  // Q10 
                                    ,Qm20, Q020, Q120, Q220  // Q20
                                    ) + 
                                 crS12(x, y, z
                                    ,Qmm1, Q0m1, Q1m1, Q2m1 // Qm1
                                    ,Qm01, Q001, Q101, Q201 // Q01
                                    ,Qm11, Q011, Q111, Q211 // Q11
                                    ,Qm21, Q021, Q121, Q221 // Q21
                                    ,Qmm2, Q0m2, Q1m2, Q2m2 // Qm2
                                    ,Qm02, Q002, Q102, Q202 // Q02
                                    ,Qm12, Q012, Q112, Q212 // Q12
                                    ,Qm22, Q022, Q122, Q222 // Q22
                                    ))
                      }               
                      contained_by{ 
                            box{
                                 <0, 0,  -1>,     // "root" of elementary cell >,
                                 <1, 1,   0>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur 
                      all_intersections
                      max_gradient _MaxG // 10.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7 //.1, 1.4, .7
                      open               
                      scale _dd
                      translate<QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1]> // float real "root" of elementary cell
                      texture{_IsoTexture}
                    }  
                #end
            #end // ixn
        #end // iyn 
   #end // izn
#end // DrawCatRomSrf Cubic based in basic cell
//===============================================================:)
//===============================================================:)
//===============================================================:)
//===============================================================:)



///*   Primitive cell /core
   //====================================//
  // Krige style interpolation function //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare _K1 = function( // basic cube core nodes __3
                        xx, yy, zz,   // xP, yP, zP: float - internally supplied
                        rx, ry, rz    // float: converted REAL coordinates of lower, left, front corner of elementary cell
                        ,_d, __d      // edge length/ double length of elementary cell
                        ,_p           // smoothing constant
                        ,_Q000, _Q010, _Q100, _Q110  // lower base
                        ,_Q001, _Q011, _Q101, _Q111  // upper base
               ){            
( // basic cube lower                                                                   // old
   (
      _Q000*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q000
      _Q010*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q100
      _Q001*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q010
      _Q011*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q110
      // basic cube upper                                                                   // 
      _Q100*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q001
      _Q110*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q101
      _Q101*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q011
      _Q111*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))   // _Q111
   )/( // weights
      exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
      // ic cube upper                                              
      exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
      exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))
   )  
)
} 
//*/       


///*   Primitive cell /core
   //====================================//
  // Krige style interpolation function //
 //  internal POVRay coordinates!!!    // 
//====================================//
#declare _K2 = function( // extended cube core nodes __2
                        xx, yy, zz,   // xP, yP, zP: float - internally supplied
                        rx, ry, rz    // float: converted REAL coordinates of lower, left, front corner of elementary cell
                        ,_d, __d      // edge length/ double length of elementary cell
                        ,_p           // smoothing constant
                        ,_Q000, _Q010, _Q100, _Q110  // lower base
                        ,_Q001, _Q011, _Q101, _Q111  // upper base
                        ,_Qm00, _Qm10, _Q200, _Q210  // X l
                        ,_Qm01, _Qm11, _Q201, _Q211  // X u
                        ,_Q0m0, _Q020, _Q1m0, _Q120  // Y l
                        ,_Q0m1, _Q021, _Q1m1, _Q121  // Y u
                        ,_Q00m, _Q01m, _Q10m, _Q11m  // Z l
                        ,_Q002, _Q012, _Q102, _Q112  // Z u                     
                        ){            
( // basic cube lower                                                                   // old
 (
  _Q000*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q000
  _Q010*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q100
  _Q001*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q010
  _Q011*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q110
  // basic cube upper                                                                   // 
  _Q100*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q001
  _Q110*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q101
  _Q101*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q011
  _Q111*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))    // _Q111

 // X extended lower  -v-                --v                 v--                        // 
 +_Q0m0*exp( -(pow((xx-rx+_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Qm00
  _Q0m1*exp( -(pow((xx-rx+_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Qm10
  _Q020*exp( -(pow((xx-rx-__d),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q200
  _Q021*exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q210
// X extended upper                                                                     // 
  _Q1m0*exp( -(pow((xx-rx+_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Qm01
  _Q1m1*exp( -(pow((xx-rx+_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Qm11
  _Q120*exp( -(pow((xx-rx-__d),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q201
  _Q121*exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q211
 // Y extended lower   -v-             --v                   v--                        // 
  _Q00m*exp( -(pow((xx-rx    ),2)+pow((yy-ry+_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q0m0
  _Q002*exp( -(pow((xx-rx    ),2)+pow((yy-ry-__d),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q020
  _Q01m*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry+_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q1m0
  _Q012*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-__d),2)+pow((rz-zz    ),2) )/pow(_p,2))+   // _Q120
// Y extended upper                                                                     // 
  _Q1m1*exp( -(pow((xx-rx+ _d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q0m1
  _Q121*exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q021
  _Q1m1*exp( -(pow((xx-rx+ _d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q1m1
  _Q121*exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+   // _Q121
 // Z extended lower  -v-                --v               v--                          // 
  _Qm00*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz+_d ),2) )/pow(_p,2))+   // _Q00m
  _Qm01*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz+_d ),2) )/pow(_p,2))+   // _Q01m
  _Qm10*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz+_d ),2) )/pow(_p,2))+   // _Q10m
  _Qm11*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz+_d ),2) )/pow(_p,2))+   // _Q11m
// Z extended upper                                                                     // 
  _Q200*exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+   // _Q002
  _Q201*exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+   // _Q012
  _Q210*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+   // _Q102
  _Q211*exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-__d),2) )/pow(_p,2))    // _Q112
)/( // weights
  exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  // ic cube upper                                              
  exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))
 // X tended lower                                               
 +exp( -(pow((xx-rx+_d ),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx+_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry    ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
 // X ended upper                                                
  exp( -(pow((xx-rx+_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx+_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry    ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz-_d ),2) )/pow(_p,2))+
 //Y tended lower                                               
  exp( -(pow((xx-rx    ),2)+pow((yy-ry+_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx    ),2)+pow((yy-ry-__d),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry+_d ),2)+pow((rz-zz    ),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-__d),2)+pow((rz-zz    ),2) )/pow(_p,2))+
 //Y ended upper                                                
  exp( -(pow((xx-rx+ _d),2)+pow((yy-ry-_d ),2)+pow((rz-zz- _d),2) )/pow(_p,2))+   // _Q0m1
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz- _d),2) )/pow(_p,2))+   // _Q021
  exp( -(pow((xx-rx+ _d),2)+pow((yy-ry-_d ),2)+pow((rz-zz- _d),2) )/pow(_p,2))+   // _Q1m1
  exp( -(pow((xx-rx-__d),2)+pow((yy-ry-_d ),2)+pow((rz-zz- _d),2) )/pow(_p,2))+   // _Q121
 //Z tended lower                                               
  exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz+ _d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz+ _d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz+ _d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz+ _d),2) )/pow(_p,2))+
 //Z ended upper                                                
  exp( -(pow((xx-rx    ),2)+pow((yy-ry    ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx    ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry    ),2)+pow((rz-zz-__d),2) )/pow(_p,2))+
  exp( -(pow((xx-rx-_d ),2)+pow((yy-ry-_d ),2)+pow((rz-zz-__d),2) )/pow(_p,2))
)  
)
} 
//*/       



   //===========================//
  //   Krige style             //
 //    Drawing facility       //
//===========================//
// Krige
/****M* Potential.inc/DrawKrigedSurf
* PURPOSE
*  Computes isosurface usin DrawKrigeSurf interpolation. Setting variable "Debug" to more than zero produces diagnostic output
* SYNOPSIS
*/
#macro DrawKrigedSurf (_trsh, _dd, _pp) // __1
/*
* INPUTS
*  _trsh - double - threshold value of isosurface
*  _dd   - double - edge length of elementary cell
*  _pp   - smoothing constant
* QVr[]  - garray - implicit attribute, nodes of computation frame
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf (), DrawKrigedSurf2 ()
*
******/
#debug concat("=>dd= ",str(_dd,6,4),"==\n")   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#local krigvar = 2;
#declare _d2d  = 2*_dd;
#declare _dd2  = pow(_dd,2);
#declare _dd3  = pow(_dd,3);
    #for(izn,1,dimension_size(QVr,3)-3)
        #debug concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n")    
        #for(iyn,1,dimension_size(QVr,2)-3)
            #for(ixn,1,dimension_size(QVr,1)-3)                               
                // do not compute "empty" cells
                #if (_trsh<min(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max(
                                    QVr[  ixn][  iyn][izn][0],   // Q1d
                                    QVr[  ixn][iyn+1][izn][0],   // Q2d
                                    QVr[ixn+1][  iyn][izn][0],   // Q3d
                                    QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                    
                                    // upper face
                                    QVr[  ixn][  iyn][izn+1][0], // Q1u
                                    QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                    QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                    QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )      
                     |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else 
                // need to compute pass multinomial coefficients (12pcs)???
                    #declare Qmmm=QVr[ixn-1][iyn-1][izn-1][0];	#declare  Q0mm=QVr[ixn  ][iyn-1][izn-1][0];	#declare  Q1mm=QVr[ixn+1][iyn-1][izn-1][0];	#declare  Q2mm=QVr[ixn+2][iyn-1][izn-1][0];	 //z=izn-1
                    #declare Qm0m=QVr[ixn-1][iyn  ][izn-1][0];	#declare  Q00m=QVr[ixn  ][iyn  ][izn-1][0];	#declare  Q10m=QVr[ixn+1][iyn  ][izn-1][0];	#declare  Q20m=QVr[ixn+2][iyn  ][izn-1][0];	          
                    #declare Qm1m=QVr[ixn-1][iyn+1][izn-1][0];	#declare  Q01m=QVr[ixn  ][iyn+1][izn-1][0];	#declare  Q11m=QVr[ixn+1][iyn+1][izn-1][0];	#declare  Q21m=QVr[ixn+2][iyn+1][izn-1][0];	          
                    #declare Qm2m=QVr[ixn-1][iyn+2][izn-1][0];	#declare  Q02m=QVr[ixn  ][iyn+2][izn-1][0];	#declare  Q12m=QVr[ixn+1][iyn+2][izn-1][0];	#declare  Q22m=QVr[ixn+2][iyn+2][izn-1][0];	          
                    				
                    #declare Qmm0=QVr[ixn-1][iyn-1][izn  ][0];	#declare  Q0m0=QVr[ixn  ][iyn-1][izn  ][0];	#declare  Q1m0=QVr[ixn+1][iyn-1][izn  ][0];	#declare  Q2m0=QVr[ixn+2][iyn-1][izn  ][0];	 //z=izn  
                    #declare Qm00=QVr[ixn-1][iyn  ][izn  ][0];	#declare  Q000=QVr[ixn  ][iyn  ][izn  ][0];	#declare  Q100=QVr[ixn+1][iyn  ][izn  ][0];	#declare  Q200=QVr[ixn+2][iyn  ][izn  ][0];	          
                    #declare Qm10=QVr[ixn-1][iyn+1][izn  ][0];	#declare  Q010=QVr[ixn  ][iyn+1][izn  ][0];	#declare  Q110=QVr[ixn+1][iyn+1][izn  ][0];	#declare  Q210=QVr[ixn+2][iyn+1][izn  ][0];	          
                    #declare Qm20=QVr[ixn-1][iyn+2][izn  ][0];	#declare  Q020=QVr[ixn  ][iyn+2][izn  ][0];	#declare  Q120=QVr[ixn+1][iyn+2][izn  ][0];	#declare  Q220=QVr[ixn+2][iyn+2][izn  ][0];	          
                    				
                    #declare Qmm1=QVr[ixn-1][iyn-1][izn+1][0];	#declare  Q0m1=QVr[ixn  ][iyn-1][izn+1][0];	#declare  Q1m1=QVr[ixn+1][iyn-1][izn+1][0];	#declare  Q2m1=QVr[ixn+2][iyn-1][izn+1][0];	 //z=izn+1
                    #declare Qm01=QVr[ixn-1][iyn  ][izn+1][0];	#declare  Q001=QVr[ixn  ][iyn  ][izn+1][0];	#declare  Q101=QVr[ixn+1][iyn  ][izn+1][0];	#declare  Q201=QVr[ixn+2][iyn  ][izn+1][0];	          
                    #declare Qm11=QVr[ixn-1][iyn+1][izn+1][0];	#declare  Q011=QVr[ixn  ][iyn+1][izn+1][0];	#declare  Q111=QVr[ixn+1][iyn+1][izn+1][0];	#declare  Q211=QVr[ixn+2][iyn+1][izn+1][0];	          
                    #declare Qm21=QVr[ixn-1][iyn+2][izn+1][0];	#declare  Q021=QVr[ixn  ][iyn+2][izn+1][0];	#declare  Q121=QVr[ixn+1][iyn+2][izn+1][0];	#declare  Q221=QVr[ixn+2][iyn+2][izn+1][0];	          
                    				
                    #declare Qmm2=QVr[ixn-1][iyn-1][izn+2][0];	#declare  Q0m2=QVr[ixn  ][iyn-1][izn+2][0];	#declare  Q1m2=QVr[ixn+1][iyn-1][izn+2][0];	#declare  Q2m2=QVr[ixn+2][iyn-1][izn+2][0];	 //z=izn+2
                    #declare Qm02=QVr[ixn-1][iyn  ][izn+2][0];	#declare  Q002=QVr[ixn  ][iyn  ][izn+2][0];	#declare  Q102=QVr[ixn+1][iyn  ][izn+2][0];	#declare  Q202=QVr[ixn+2][iyn  ][izn+2][0];	          
                    #declare Qm12=QVr[ixn-1][iyn+1][izn+2][0];	#declare  Q012=QVr[ixn  ][iyn+1][izn+2][0];	#declare  Q112=QVr[ixn+1][iyn+1][izn+2][0];	#declare  Q212=QVr[ixn+2][iyn+1][izn+2][0];	          
                    #declare Qm22=QVr[ixn-1][iyn+2][izn+2][0];	#declare  Q022=QVr[ixn  ][iyn+2][izn+2][0];	#declare  Q122=QVr[ixn+1][iyn+2][izn+2][0];	#declare  Q222=QVr[ixn+2][iyn+2][izn+2][0];	
// testing..
#if (Debug>0)

    #local sx =  (QVr[ixn][iyn][izn][2]+QVr[ixn  ][iyn+1][izn  ][2])/2;
    #local sy =  (QVr[ixn][iyn][izn][3]+QVr[ixn  ][iyn  ][izn+1][3])/2;
    #local sz = -(QVr[ixn][iyn][izn][1]+QVr[ixn+1][iyn  ][izn  ][1])/2;
    #local CV =( _K1(sx, sy, sz,
                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                    _dd, _d2d, _pp                      
                    ,Q000, Q010, Q100, Q110  // lower base
                    ,Q001, Q011, Q101, Q111  // upper base
                    ,Qm00, Qm10, Q200, Q210  // X l
                    ,Qm01, Qm11, Q201, Q211  // X u
                    ,Q0m0, Q020, Q1m0, Q120  // Y l
                    ,Q0m1, Q021, Q1m1, Q121  // Y u
                    ,Q00m, Q01m, Q10m, Q11m  // Z l
                    ,Q002, Q012, Q102, Q112  // Z u                     
                    ));
    #debug concat("at <", vstr(3, <-sz,sx,sy>,"; ",4,2), ">\tintpd V(r)= ", str(CV, 15,7), ", but cmptd  V(r)= ", str((Q000+Q100+Q010+Q110+Q001+Q011+Q101+Q111)/8, 15, 7), "\n" )                        
    sphere{
       < sx, sy, sz>, _dd/4
    // texture{pigment{color rgb <1,abs(QVr[ixn][iyn][izn][3]),CV>} finish{emission .1}}
       texture{pigment{color rgb abs(CV) } finish{emission .1}}
    }                 
#end // Debug
                  isosurface{
                      function{
                           (
                              #switch(krigvar)
                              #case (1) _K1(
                                         x, y, z,
                                         QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                                         _dd, _d2d, _pp                      
                                        ,Q000, Q010, Q100, Q110  // lower base
                                        ,Q001, Q011, Q101, Q111  // upper base
                                     )
                                     #break
                              #case (2) _K2(
                                         x, y, z,
                                         QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
                                         _dd, _d2d, _pp                      
                                        ,Q000, Q010, Q100, Q110  // lower base
                                        ,Q001, Q011, Q101, Q111  // upper base
                                        ,Qm00, Qm10, Q200, Q210  // X l
                                        ,Qm01, Qm11, Q201, Q211  // X u
                                        ,Q0m0, Q020, Q1m0, Q120  // Y l
                                        ,Q0m1, Q021, Q1m1, Q121  // Y u
                                        ,Q00m, Q01m, Q10m, Q11m  // Z l
                                        ,Q002, Q012, Q102, Q112  // Z u                     
                                     )
                                     #break
                              #end  
                           )
                      }               
                      contained_by{ 
                            box{
                                 <QVr[  ixn][  iyn][  izn][2], QVr[  ixn][  iyn][  izn][3], -QVr[  ixn][  iyn][izn+1][1]>,     // "root" of elementary cell >,
                                 <QVr[ixn+1][iyn+1][izn+1][2], QVr[ixn+1][iyn+1][izn+1][3], -QVr[ixn+1][iyn+1][izn  ][1]>      // "root" of elementary cell
                            }
                      }
                      threshold _trsh
                      accuracy _Accur
                      all_intersections
                      max_gradient _MaxG // 100.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7 // .1, 1.4, .7
                      open               
                      texture{_IsoTexture}
                    }  
                #end
            #end // ixn
        #end // iyn 
   #end // izn
#end // DrawKrigedSurf kriging based in basic cell
//================================================:)
//================================================:)
//================================================:)
//================================================:)

/****f* Potential.inc/_K3a
* PURPOSE
*  Computes KDE approximation using gaussian interpolation. 
*  Whole interpolation is split in three functions, _K3a, _K3b, _K3den due to limitation imposed on number of arguments.
*  Total output equals: [(_K3a()+_K3b())/_K3den()] - denominator is computed in _K3den () fucnction, 
*  Model is computed in reduced space, ie. rx,ry,rz is assumed zero.
*  Setting variable "Debug" to more than zero produces diagnostic output.
* SYNOPSIS
*/
#declare _K3a = function(             // Full cube core nodes, internal part 1; __2
                        xx, yy, zz   // xP, yP, zP: float - internally supplied
                        ,_d, __d      // edge length/ double length of elementary cell
                        ,_p           // smoothing constant
                        ,_Q000, _Q010, _Q100, _Q110  // lower base
                        ,_Q001, _Q011, _Q101, _Q111  // upper base
                        ,_Qm00, _Qm10, _Q200, _Q210  // X l
                        ,_Qm01, _Qm11, _Q201, _Q211  // X u
                        ,_Q0m0, _Q020, _Q1m0, _Q120  // Y l
                        ,_Q0m1, _Q021, _Q1m1, _Q121  // Y u
                        ,_Q00m, _Q01m, _Q10m, _Q11m  // Z l
                        ,_Q002, _Q012, _Q102, _Q112  // Z u                     
                        )            
/*
* INPUTS
*  xx ,yy, zz - double - supplied by POV-Ray, current point in space
*  _d, __d    - double - edge length/ double length of elementary cell
*  _p         - double - smoothing constant,
* Quvw        - double - values in nodes. uvw->{m,0,1,2}: m:=i-1, 0:=i, 1:=i+1, 2:=i+2 
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf (), _K3b
*
******/                  
{
( // basic cube lower                                                                   // old
  _Q000*exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q000
  _Q010*exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q100
  _Q001*exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q010
  _Q011*exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q110
  // basic cube upper                                                           // 
  _Q100*exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q001
  _Q110*exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q101
  _Q101*exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q011
  _Q111*exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+    // _Q111

 // X extended lower                                 v--                        // 
  _Q0m0*exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Qm00
  _Q0m1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Qm10
  _Q020*exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q200
  _Q021*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q210
// X extended upper                                                             // 
  _Q1m0*exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Qm01
  _Q1m1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Qm11
  _Q120*exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q201
  _Q121*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q211
 // Y extended lower  -             --               v--                        // 
  _Q00m*exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q0m0
  _Q002*exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q020
  _Q01m*exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q1m0
  _Q012*exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   // _Q120
// Y extended upper                                                                    // 
  _Q1m1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q0m1
  _Q121*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q021
  _Q1m1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q1m1
  _Q121*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   // _Q121
 // Z extended lower                                --                          // 
  _Qm00*exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   // _Q00m
  _Qm01*exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   // _Q01m
  _Qm10*exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   // _Q10m
  _Qm11*exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   // _Q11m
// Z extended upper                                                             // 
  _Q200*exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   // _Q002
  _Q201*exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   // _Q012
  _Q210*exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   // _Q102
  _Q211*exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))    // _Q112  
)
} 
//function _K3a () */       

/****f* Potential.inc/_K3b
* PURPOSE
*  Computes KDE approximation using gaussian interpolation. 
*  Whole interpolation is split in three functions, _K3a, _K3b, _K3den due to limitation imposed on number of arguments.
*  Total output equals: [(_K3a()+_K3b())/_K3den()] - denominator is computed in _K3dem () fucnction, 
*  Model is computed in reduced space, ie. rx,ry,rz is assumed zero.
*  Setting variable "Debug" to more than zero produces diagnostic output.
* SYNOPSIS
*/
#declare _K3b = function(             // Full cube core nodes, internal part 1; __2
                        xx, yy, zz   // xP, yP, zP: float - internally supplied
                        ,_d, __d      // edge length/ double length of elementary cell
                        ,_p           // smoothing constant
                        ,_Qmmm, _Q0mm, _Q1mm, _Q2mm  // lower dozen       __m
                        ,_Qm0m,               _Q20m  // lower dozen       __m
                        ,_Qm1m,               _Q21m  // lower dozen       __m
                        ,_Qm2m, _Q02m, _Q12m, _Q22m  // lower dozen       __m
                        ,_Qmm2, _Q0m2, _Q1m2, _Q2m2  // upper dozen       __2
                        ,_Qm02,               _Q202  // upper dozen       __2
                        ,_Qm12,               _Q212  // upper dozen       __2
                        ,_Qm22, _Q022, _Q122, _Q222  // upper dozen       __2
                        ,_Qmm0, _Q2m0, _Q220, _Qm20  // 0 level fourth    __0
                        ,_Qmm1, _Q2m1, _Q221, _Qm21  // 1 level fourth    __1
                        )            
/*
* INPUTS
*  xx ,yy, zz - double - supplied by POV-Ray, current point in space
*  _d, __d    - double - edge length/ double length of elementary cell
*  _p         - double - smoothing constant,
* Quvw        - double - values in nodes. uvw->{m,0,1,2}: m:=i-1, 0:=i, 1:=i+1, 2:=i+2 
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf (), _K3b
*
******/
{
( // lower dozen  __m                                                                 
  _Qmmm*exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q0mm*exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  _Q1mm*exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  _Q2mm*exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm0m*exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q20m*exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm1m*exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q21m*exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm2m*exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q02m*exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  _Q12m*exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  _Q22m*exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   

// upper dozen  __2
  _Qmm2*exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q0m2*exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  _Q1m2*exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  _Q2m2*exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm02*exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q202*exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm12*exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q212*exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm22*exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q022*exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  _Q122*exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  _Q222*exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   

// 0 level fourth __0
  _Qmm0*exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q220*exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Q2m0*exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm20*exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   

// 1 level fourth __1                                                           
  _Qmm1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  _Q221*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Q2m1*exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  _Qm21*exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))      
)
} 
//function _K3b () 
//Quvt-><v;t;(-u)>=<xx-V*d,        yy-T*d,          -zz-U*d>  u,v,t={-1m, 0, 1, 2}    

/****f* Potential.inc/_K3den
* PURPOSE
*  Computes KDE approximation using gaussian interpolation. 
*  Whole interpolation is split in three functions, _K3a, _K3b, _K3den due to limitation imposed on number of arguments.
*  Total output equals: [(_K3a()+_K3b())/_K3den()] - denominator is computed in _K3den () function, 
*  Model is computed in reduced space, ie. rx,ry,rz is assumed zero.
*  Setting variable "Debug" to more than zero produces diagnostic output.
* SYNOPSIS
*/
#declare _K3den = function(           // Full cube core nodes, internal part 1; __2
                        xx, yy, zz    // xP, yP, zP: float - internally supplied
                        ,_d, __d      // edge length/ double length of elementary cell
                        ,_p           // smoothing constant
                        )            
/*
* INPUTS
*  xx, yy, zz - double - supplied by POV-Ray, current point in space
*  _d, __d    - double - edge length/ double length of elementary cell
*  _p         - double - smoothing constant,
* Quvw        - double - values in nodes. uvw->\m,0,1,2\: m:=i-1, 0:=i, 1:=i+1, 2:=i+2 
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf (), _K3a (), _K3b ()
*
******/                  
{
( //  basic cube lower                                                              
  exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  // basic cube upper                                                     
  exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   

  // X extended lower                                 v--                 
  exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  // X extended upper                                                     
  exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  // Y extended lower  -             --               v--                 
  exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  // Y extended upper                                                     
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
       // Z extended lower                                --              
       // "corners"
  exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  // Z extended upper                                                     
  exp( -(pow((xx    ),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+    
 
        // lower dozen  __m
  exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy+ _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   

  // upper dozen  __2
  exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx    ),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx- _d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz    ),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz- _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy-__d),2)+pow((-zz-__d),2) )/pow(_p,2))+   

  // 0 level fourth __0
  exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy    ),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy    ),2)+pow((-zz+ _d),2) )/pow(_p,2))+   

  // 1 level fourth __1                                                           
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx+ _d),2)+pow((yy- _d),2)+pow((-zz-__d),2) )/pow(_p,2))+   
  exp( -(pow((xx-__d),2)+pow((yy- _d),2)+pow((-zz+ _d),2) )/pow(_p,2))      
)
} 
//function _K3den () */       
//<v;t;(-u)>=<xx-V*d,        yy-T*d,          -zz-U*d>  u,v,t=/-1m, 0, 1, 2/    


   //===========================//
  //   Krige style             //
 //    Drawing facility       //
//===========================//
// Krige
/****M* Potential.inc/DrawKrigedSurf2
* PURPOSE
*  Computes isosurface using gaussian KDE interpolation. Setting variable "Debug" to more than zero produces diagnostic output
*  Features full frame (ie. all 64 neighbours) in reduced coordinates.
* SYNOPSIS
*/
#macro DrawKrigedSurf2 (_trsh, _dd, _pp) // __1
/*
* INPUTS
*  _trsh - double - threshold value of isosurface
*  _dd   - double - edge length of elementary cell
*  _pp   - smoothing constant
* QVr[]  - garray - implicit attribute, nodes of computation frame
* OUTPUTS
*  None, 
* SIDE EFFECTS
*  None or unknown
* SEE ALSO                   
*  DrawCubicInterpolSrf (), Draw01CubicInterpolSrf (), DrawKrigedSurf ()
*
******/
#debug concat("=>dd= ",str(_dd,6,4),"==\n")   
#declare ttr_ = concat(" Trsh= ", str(_trsh,6,4));   

#declare _d2d  = 2*_dd;
#declare _dd2  = pow(_dd,2);
#declare _dd3  = pow(_dd,3);
    #for(izn,1,dimension_size(QVr,3)-3)
        #debug concat(" Processing layer.. [", str(izn,3,0) "] by treshold", ttr_ ," \n")    
        #for(iyn,1,dimension_size(QVr,2)-3)
            #for(ixn,1,dimension_size(QVr,1)-3)                               
                // do not compute "empty" cells
                #if ((_trsh<min(  // lower face
                                 QVr[  ixn][  iyn][izn][0],   // Q1d
                                 QVr[  ixn][iyn+1][izn][0],   // Q2d
                                 QVr[ixn+1][  iyn][izn][0],   // Q3d
                                 QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                 
                                 // upper face
                                 QVr[  ixn][  iyn][izn+1][0], // Q1u
                                 QVr[  ixn][iyn+1][izn+1][0], // Q2u
                                 QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                 QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                           ) 
                    | _trsh>max( // lower face
                                 QVr[  ixn][  iyn][izn][0],   // Q1d
                                 QVr[  ixn][iyn+1][izn][0],   // Q2d
                                 QVr[ixn+1][  iyn][izn][0],   // Q3d
                                 QVr[ixn+1][iyn+1][izn][0],   // Q4d
                                 
                                 // upper face
                                 QVr[  ixn][  iyn][izn+1][0], // Q1u
                                 QVr[  ixn][iyn+1][izn+1][0], // Q2u 
                                 QVr[ixn+1][  iyn][izn+1][0], // Q3u
                                 QVr[ixn+1][iyn+1][izn+1][0]  // Q4u
                            )) 
                       |  (QVr[ixn][iyn][izn][4])     
                    ) // do nothing
//                #debug concat(ttr_ " Cell [", str(ixn,3,0), "Cell [", str(ixn,3,0),"][", str(iyn,3,0),"][",str(izn,3,0),"] ",str(QVr[ixn][iyn][izn][4],3,0), "  ommited, \n")    
                #else 
                // need to compute pass multinomial coefficients (12pcs)???
                    #declare Qmmm=QVr[ixn-1][iyn-1][izn-1][0];	#declare  Q0mm=QVr[ixn  ][iyn-1][izn-1][0];	#declare  Q1mm=QVr[ixn+1][iyn-1][izn-1][0];	#declare  Q2mm=QVr[ixn+2][iyn-1][izn-1][0];	 //z=izn-1
                    #declare Qm0m=QVr[ixn-1][iyn  ][izn-1][0];	#declare  Q00m=QVr[ixn  ][iyn  ][izn-1][0];	#declare  Q10m=QVr[ixn+1][iyn  ][izn-1][0];	#declare  Q20m=QVr[ixn+2][iyn  ][izn-1][0];	          
                    #declare Qm1m=QVr[ixn-1][iyn+1][izn-1][0];	#declare  Q01m=QVr[ixn  ][iyn+1][izn-1][0];	#declare  Q11m=QVr[ixn+1][iyn+1][izn-1][0];	#declare  Q21m=QVr[ixn+2][iyn+1][izn-1][0];	          
                    #declare Qm2m=QVr[ixn-1][iyn+2][izn-1][0];	#declare  Q02m=QVr[ixn  ][iyn+2][izn-1][0];	#declare  Q12m=QVr[ixn+1][iyn+2][izn-1][0];	#declare  Q22m=QVr[ixn+2][iyn+2][izn-1][0];	          
                    				
                    #declare Qmm0=QVr[ixn-1][iyn-1][izn  ][0];	#declare  Q0m0=QVr[ixn  ][iyn-1][izn  ][0];	#declare  Q1m0=QVr[ixn+1][iyn-1][izn  ][0];	#declare  Q2m0=QVr[ixn+2][iyn-1][izn  ][0];	 //z=izn  
                    #declare Qm00=QVr[ixn-1][iyn  ][izn  ][0];	#declare  Q000=QVr[ixn  ][iyn  ][izn  ][0];	#declare  Q100=QVr[ixn+1][iyn  ][izn  ][0];	#declare  Q200=QVr[ixn+2][iyn  ][izn  ][0];	          
                    #declare Qm10=QVr[ixn-1][iyn+1][izn  ][0];	#declare  Q010=QVr[ixn  ][iyn+1][izn  ][0];	#declare  Q110=QVr[ixn+1][iyn+1][izn  ][0];	#declare  Q210=QVr[ixn+2][iyn+1][izn  ][0];	          
                    #declare Qm20=QVr[ixn-1][iyn+2][izn  ][0];	#declare  Q020=QVr[ixn  ][iyn+2][izn  ][0];	#declare  Q120=QVr[ixn+1][iyn+2][izn  ][0];	#declare  Q220=QVr[ixn+2][iyn+2][izn  ][0];	          
                    				
                    #declare Qmm1=QVr[ixn-1][iyn-1][izn+1][0];	#declare  Q0m1=QVr[ixn  ][iyn-1][izn+1][0];	#declare  Q1m1=QVr[ixn+1][iyn-1][izn+1][0];	#declare  Q2m1=QVr[ixn+2][iyn-1][izn+1][0];	 //z=izn+1
                    #declare Qm01=QVr[ixn-1][iyn  ][izn+1][0];	#declare  Q001=QVr[ixn  ][iyn  ][izn+1][0];	#declare  Q101=QVr[ixn+1][iyn  ][izn+1][0];	#declare  Q201=QVr[ixn+2][iyn  ][izn+1][0];	          
                    #declare Qm11=QVr[ixn-1][iyn+1][izn+1][0];	#declare  Q011=QVr[ixn  ][iyn+1][izn+1][0];	#declare  Q111=QVr[ixn+1][iyn+1][izn+1][0];	#declare  Q211=QVr[ixn+2][iyn+1][izn+1][0];	          
                    #declare Qm21=QVr[ixn-1][iyn+2][izn+1][0];	#declare  Q021=QVr[ixn  ][iyn+2][izn+1][0];	#declare  Q121=QVr[ixn+1][iyn+2][izn+1][0];	#declare  Q221=QVr[ixn+2][iyn+2][izn+1][0];	          
                    				
                    #declare Qmm2=QVr[ixn-1][iyn-1][izn+2][0];	#declare  Q0m2=QVr[ixn  ][iyn-1][izn+2][0];	#declare  Q1m2=QVr[ixn+1][iyn-1][izn+2][0];	#declare  Q2m2=QVr[ixn+2][iyn-1][izn+2][0];	 //z=izn+2
                    #declare Qm02=QVr[ixn-1][iyn  ][izn+2][0];	#declare  Q002=QVr[ixn  ][iyn  ][izn+2][0];	#declare  Q102=QVr[ixn+1][iyn  ][izn+2][0];	#declare  Q202=QVr[ixn+2][iyn  ][izn+2][0];	          
                    #declare Qm12=QVr[ixn-1][iyn+1][izn+2][0];	#declare  Q012=QVr[ixn  ][iyn+1][izn+2][0];	#declare  Q112=QVr[ixn+1][iyn+1][izn+2][0];	#declare  Q212=QVr[ixn+2][iyn+1][izn+2][0];	          
                    #declare Qm22=QVr[ixn-1][iyn+2][izn+2][0];	#declare  Q022=QVr[ixn  ][iyn+2][izn+2][0];	#declare  Q122=QVr[ixn+1][iyn+2][izn+2][0];	#declare  Q222=QVr[ixn+2][iyn+2][izn+2][0];	
// testing..
#if (Debug>0)

    #local sx =  (QVr[ixn][iyn][izn][2]+QVr[ixn  ][iyn+1][izn  ][2])/2;
    #local sy =  (QVr[ixn][iyn][izn][3]+QVr[ixn  ][iyn  ][izn+1][3])/2;
    #local sz = -(QVr[ixn][iyn][izn][1]+QVr[ixn+1][iyn  ][izn  ][1])/2;
    #local CV =(( _K3a(sx, sy, sz,
                    _dd, _d2d, _pp                      
                    ,Q000, Q010, Q100, Q110  // lower base
                    ,Q001, Q011, Q101, Q111  // upper base
                    ,Qm00, Qm10, Q200, Q210  // X l
                    ,Qm01, Qm11, Q201, Q211  // X u
                    ,Q0m0, Q020, Q1m0, Q120  // Y l
                    ,Q0m1, Q021, Q1m1, Q121  // Y u
                    ,Q00m, Q01m, Q10m, Q11m  // Z l
                    ,Q002, Q012, Q102, Q112  // Z u                     
                    )
                    +
                 _K3b(sx, sy, sz,
                   _dd, _d2d, _pp                      
                  ,Qmmm, Q0mm, Q1mm, Q2mm  // lower dozen       __m
                  ,Qm0m,             Q20m  // lower dozen       __m
                  ,Qm1m,             Q21m  // lower dozen       __m
                  ,Qm2m, Q02m, Q12m, Q22m  // lower dozen       __m
                  ,Qmm2, Q0m2, Q1m2, Q2m2  // upper dozen       __2
                  ,Qm02,             Q202  // upper dozen       __2
                  ,Qm12,             Q212  // upper dozen       __2
                  ,Qm22, Q022, Q122, Q222  // upper dozen       __2
                  ,Qmm0, Q2m0, Q220, Qm20  // 0 level fourth    __0
                  ,Qmm1, Q2m1, Q221, Qm21  // 1 level fourth    __1
               ))/_K3den(sx, sy, sz, _dd, _d2d, _pp));
    #debug concat("at <", vstr(3, <-sz,sx,sy>,"; ",4,2), ">\tintpd V(r)= ", str(CV, 15,7), ", but cmptd  V(r)= ", str((Q000+Q100+Q010+Q110+Q001+Q011+Q101+Q111)/8, 15, 7), "\n" )                        
    sphere{
       < sx, sy, sz>, _dd/4
    // texture{pigment{color rgb <1,abs(QVr[ixn][iyn][izn][3]),CV>} finish{emission .1}}
       texture{pigment{color rgb abs(CV) } finish{emission .1}}
    }                 
#end // Debug
                  isosurface{
                      function{
                           (
                             _K3a(x, y, z,
                                _dd, _d2d, _pp                      
                               ,Q000, Q010, Q100, Q110  // lower base
                               ,Q001, Q011, Q101, Q111  // upper base
                               ,Qm00, Qm10, Q200, Q210  // X l
                               ,Qm01, Qm11, Q201, Q211  // X u
                               ,Q0m0, Q020, Q1m0, Q120  // Y l
                               ,Q0m1, Q021, Q1m1, Q121  // Y u
                               ,Q00m, Q01m, Q10m, Q11m  // Z l
                               ,Q002, Q012, Q102, Q112  // Z u                     
                            )
                            +
                            _K3b(x, y, z,
                                _dd, _d2d, _pp                      
                                ,Qmmm, Q0mm, Q1mm, Q2mm  // lower dozen       __m
                                ,Qm0m,             Q20m  // lower dozen       __m
                                ,Qm1m,             Q21m  // lower dozen       __m
                                ,Qm2m, Q02m, Q12m, Q22m  // lower dozen       __m
                                ,Qmm2, Q0m2, Q1m2, Q2m2  // upper dozen       __2
                                ,Qm02,             Q202  // upper dozen       __2
                                ,Qm12,             Q212  // upper dozen       __2
                                ,Qm22, Q022, Q122, Q222  // upper dozen       __2
                                ,Qmm0, Q2m0, Q220, Qm20  // 0 level fourth    __0
                                ,Qmm1, Q2m1, Q221, Qm21  // 1 level fourth    __1
                             )                                                        
                             )/_K3den(x, y, z, _dd, _d2d, _pp)
                      }               
                      contained_by{ 
                            box{
                                 <  0,   0,    0>,     // elementary
                                 <_dd, _dd, -_dd>      // box
                            }
                      }
                      threshold _trsh
                      accuracy _Accur
                      all_intersections
                      max_gradient _MaxG // 100.1  
                      evaluate _MaxG*.7, sqrt(1/.7), .7 // .1, 1.4, .7
                      open               
                      translate < QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1]>  // float real "root" of elementary cell
                      texture{_IsoTexture}
                    }  
                #end
            #end // ixn
        #end // iyn 
   #end // izn
#end // DrawKrigedSurf2 kriging based in basic cell
///////////////////////////////////////////////////// 
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

#else
 DebugMsg("Module Potential recalled\n",1)
 #ifndef (_CallStack )
   #declare _CallStack = "Call stack:\n"  
 #end
 #declare _CallStack = concat(_CallStack, _ModuleDescription,"\n")
#end // ...#ifndef _Potential_INC
// brutal reserve
//                        InterCubic( x,y,z,    // conversion from real to internal representation
//                                    QVr[ixn][iyn][izn][2], QVr[ixn][iyn][izn][3], -QVr[ixn][iyn][izn][1],  // float real "root" of elementary cell
//                                    _dd
//                        ) 
