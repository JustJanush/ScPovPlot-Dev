/****h* ScPovPlot3D/Cameras.inc
* PURPOSE
*   Very important toolkit defining some handy cameras
*   TODO: still a lot!
*   |html <hr width=50% align="left">
* VERSION
*  4.0.5, 2021-05-19 - tested on PovRay 3.7.0
* AUTHOR
*  Janusz Opi³a Ph.D.
*   jmo{at}agh.edu.pl, janusz.opila{at}gmail.com
*   Dept. of Applied Informatics, 
*   href:https://www.facebook.com/KatedraInformatykiStosowanejWZAGH/
*   href:http://kis.zarz.agh.edu.pl/
*   AGH University of Science & Technology, Cracow, Poland href:http://www.zarz.agh.edu.pl/English/index.asp
*   Maintained by Janusz Opi³a Ph.D.
*   Homepage: http://scpovplot3d.sourceforge.net   
* COPYRIGHT
*  GNU GPL v.3 License
*  (c) 2012-now by Janusz Opi³a Ph.D.         
*  AGH University of Science and Technology
* HISTORY
*  2016-10-12, v3.2.0.7, IntelligentEyeT - Target was not applied
*  2016-03-02, v3.2.0.6
*    - light_source added in InsertBarKeyFrItem, InsertBarKeyFrItemBelow definition
*  2016-02-12, v3.2.0.5 
*    - InsertKeyItem macro evolved into two: InsertBarKeyItem, InsertCylKeyItem. Minor improvements.
*    - CameraTarget may be now re-setted.                     
*  2016-02-02, v3.2.0.3 - added InsertKeyItem() macro
*  2021-04-11, v4.0.4 - minor bug in 'range' control for 'CameraAngiel()'
*  2021-05-19, v4.0.5 - simple default perspective cameras FCamera() and FTiltedCamera() added;
****
  end of RoboDoc comment
****/

#ifndef(Cameras_INC)
#declare Cameras_INC = "2018-09-17"; // last modification date
#version 3.7;
// common includes & definitions
#declare _FileName   = "Cameras.inc";
#declare _FileVerMaj = "4.0.0";
#declare _FileVerMin = Cameras_INC;

#debug concat("\n[==> ", _FileName,", ver: ", _FileVerMaj, ", build:  ", _FileVerMin, " <==]\n")
#include "math.inc"

#ifndef (Debug) // Just 4 safety
   #declare Debug=0;
#end


/****v* Cameras.inc/SceneMinX, SceneMaxX, SceneMinY, SceneMaxY, SceneMinZ, SceneMaxZ, CameraTarget, CameraShift, CameraAngle
* PURPOSE
*   Internal variables set by relevant macros or automatically by scene itself
* SEE ALSO
*  SetCameraTarget ()
*  SetCameraShift ()
*  SetCameraAngle ()
* SOURCE 
*/
#declare SceneMinX    =  0.0;        // these variables defines bounding box for the whole scene
#declare SceneMaxX    =  0.0;        // they are utilized by some macros to determine camera parameters
#declare SceneMinY    =  0.0;
#declare SceneMaxY    =  0.0;
#declare SceneMinZ    =  0.0;
#declare SceneMaxZ    =  0.0; 
#declare CameraAngle  =  35 ;        // camera viewing angle
#declare CameraAngleSet = false;     // default setting is "no explicit setting"
#declare CameraTarget = <0, 0, 0>;   // virtual point Camera looks at
#declare CameraShift  = <0, 0, 0>;   // additional shift in camera position
/******/


/****f* Cameras.inc/SetCameraTarget
* PURPOSE
*  defines point in space to watch on
*  WARNING: POVRay coordinates!
* SYNOPSIS
*/
#macro SetCameraTarget (XX, YY, ZZ)
/*
 * INPUTS
 *  XX, YY, ZZ - POVRay coordinates of the target point
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 ****/
//    #ifndef(CameraTarget_SET) // obsolete
        #declare CameraTarget=<XX, YY, ZZ>;
        #declare CameraTarget_SET=1;
//    #end
#end

// common defs, internal variables
#declare  lam = radians(25);
#declare  phi = radians(35);
#declare  r   = 5;
#declare  camera_x = r*cos(phi)*sin(lam);
#declare  camera_y = r*sin(phi);
#declare  camera_z =-r*cos(phi)*cos(lam);
#declare  Perspective=true;
#declare  _LabelWidth=0;

/****f* Cameras.inc/SetLabelWidth
 * PURPOSE
 *  defines intended Label Width
 * SYNOPSIS
 */
#macro SetLabelWidth (_LW)
/* 
 * INPUTS
 *  positive float
 * SEE ALSO
 *  none
 ****/
   #declare _LabelWidth=_LW;
#end

/****f* Cameras.inc/SetPerspective
 * PURPOSE
 *  defines perspective camera
 * SYNOPSIS
 */
#macro SetPerspective  ()
/* 
 * INPUTS
 *  no inputs
 * SEE ALSO
 *  SetOrthographic  ()
 ****/
   #declare Perspective=true;
#end

/****f* Cameras.inc/SetOrthographic
* PURPOSE
*  defines orthografic camera
* SYNOPSIS
SetOrthographic (float OSc)
* INPUTS
*  OSc - float: scaling factor
* SEE ALSO
*  SetPerspective  ()
****/
#macro SetOrthographic(OSc)
   #declare Perspective=false;       
   #declare OrthoScale=OSc;
#end                 

/****f* Cameras.inc/SetCameraShift  [(3DVector ShftVec)]
 * PURPOSE
 *  shifts vector and adjust default LookAt point: CameraTarget=CameraTarget+CameraShift 
 * SYNOPSIS
 */
#macro SetCameraShift (ShftVec) // Shift vector adjust default LookAt point: CameraTarget=CameraTarget+CameraShift
/*
 * INPUTS 
 *  ShftVec - valid 3D vector expression in REAL coordinates
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraTarget ()
 ****/
   #declare CameraShift=ShftVec;
#end


/****f* Cameras.inc/SetCameraAngle 
* PURPOSE
*  tries to guess best target point and adjust it by CameraShift, see SetCameraShift(ShftVec)
* SYNOPSIS
*/
#macro SetCameraAngle (_ang)   // sets camera horizonta viewing angle
/*
 * INPUTS
 *  Angle - valid float expression in degrees
 * SEE ALSO
 *  SetCameraShift ()
 *  SetCameraTarget ()
 ****/
    #declare CameraAngle=_ang;
    #declare CameraAngleSet=true; // camera angle is explicitly set
#if (Debug>1) #debug concat("Cameras.inc: horizontal viewing angle has been set to ", str(CameraAngle,6,1)," [deg]\n") #end
#end


/****M* Cameras.inc/PrepareCamera 
* PURPOSE
*  Element of main camera subsystem. 
*  Ought to be placed before scene description as macros could be informed about camera position: 
*  this is for future smart and sophisticated extensions ;) Must be followed by call to CameraOn () macro
* SYNOPSIS                        
*/
#macro PrepareCamera (_lam, _phi, _r)
  // lam : longitude: angle from '-z' axis(PovRay) ie. 'x' axis in real world
  // phi : angle above POvRay '-z*x' plane -> X*Y plane in real world: latitude
  // r   : distance from XYZ origin
/*
 * INPUTS
 *  _lam, _phi, _r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
 *                 possibly set/reset by SetCameraTarget () macro. If _r<>1 then SET CameraOn (r) with r=1!
 *                 or unexpected behaviour may be observed.   
 * SEE ALSO
 *  CameraOn ()
 *  setPerspective ()
 *  SetOrthographic ()
 *  SetCameraTarget ()
 ****/   

  #local  lam = radians(_lam);
  #local  phi = radians(_phi);
// !! PovRay coords
  #declare  camera_x = _r*cos(phi)*sin(lam);
  #declare  camera_y = _r*sin(phi);
  #declare  camera_z =-_r*cos(phi)*cos(lam);
#end


/****M* Cameras.inc/CameraOn 
* PURPOSE
*  Element of main camera subsystem.
*  It tries to guess best position and viewing angle from the scene data and
*  tries to guess best target point and adjust it by CameraShift, see SetCameraShift ()
* SYNOPSIS
CameraOn (float Dist)
* INPUTS
*  Dist - float: distance from Target if using perspective camera(SetPerpective () set)
* SEE ALSO
*  setPerspective ()
*  SetOrthographic ()
*  SetCameraTarget ()
****/   
#macro CameraOn(Dist)
  #ifdef(CameraTarget_SET)
      #declare Xcent=CameraTarget.x;
      #declare Ycent=CameraTarget.y;
      #declare Zcent=CameraTarget.z;
  #else
      #declare Xcent=(SceneMinX+SceneMaxX)/2;  // hypothetic center of entire chart
      #declare Ycent=(SceneMinY+SceneMaxY)/2;
      #declare Zcent=(SceneMinZ+SceneMaxZ)/2;
  #end

  #declare camera_x=Dist*camera_x + Xcent + CameraShift.y;     // new shifted location of camera
  #declare camera_y=Dist*camera_y + Ycent + CameraShift.z;
  #declare camera_z=Dist*camera_z + Zcent - CameraShift.x;

  #declare angiel=1.4*max(Xcent, Ycent, Zcent)/VDist(<camera_x, camera_y, camera_z>, <Xcent, Ycent, Zcent>);
//1.4*tangens of angle between camera and most distant chart point
   
  camera{
     #if(Perspective)
         perspective
         location <camera_x, camera_y, camera_z>  // coordinates of the camera biased by CameraShift
         look_at <Xcent, Ycent, Zcent>            // camera focuses at that point
         up y
         right image_width/image_height*x  
         #if (CameraAngleSet)
            angle clip(CameraAngle, .1, 179.9)
         #else
            angle clip(atan(angiel)*180/pi, 10, 170)
         #end
         #if (Debug>0) #debug concat("\nView angle= ", str( clip(atan(angiel)*180/pi, 10, 170), 10, 6 ), "\n") #end
     #else
         orthographic
         location <camera_x, camera_y, camera_z>  // coordinates of the camera, scaled by "Dist"
         look_at <Xcent, Ycent, Zcent>            // camera focuses at that point
         up OrthoScale*y
         // up OrthoScale*vcross(<Xcent-camera_x,Ycent-camera_y,Zcent-camera_z>,<Zcent-camera_z,0.0,-Xcent+camera_x>)  // hack
         right OrthoScale*(image_width/image_height)*x
     #end
} //camera
#end

/****M* Cameras.inc/IntelligentEye 
* PURPOSE
*  tries to guess best target point, NO adjustment to camera position
* SYNOPSIS
IntelligentEye (lam, phi, r)
* INPUTS
*  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
*                 possibly set/reset by SetCameraTarget () macro
* SEE ALSO
*  SetCameraAngle ()
*  SetCameraTarget ()
*  IntelligentEyeT ()
****/
#macro IntelligentEye (lam, phi, r)
// lam : longitude
// phi : latitude
// r   : distance from XYZ origin
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam);
  #declare  camera_y = r*sin(phi);
  #declare  camera_z =-r*cos(phi)*cos(lam);
  #declare Xcent=(SceneMinX+SceneMaxX)/2;  // hypothetic center of entire chart
  #declare Ycent=(SceneMinY+SceneMaxY)/2;
  #declare Zcent=(SceneMinZ+SceneMaxZ)/2;

  camera{
     location <camera_x+Xcent,camera_y+Ycent, camera_z+Zcent>        // coordinates of the camera
     right x*image_width/image_height
     up y
     look_at <Xcent, Ycent, Zcent>        // camera focuses at that point
} //camera
#end
/********************************************/


/****M* Cameras.inc/IntelligentEyeT 
* PURPOSE
*  tries to guess best target point AND adjust camera position by CameraShift in REAL coords set by SetCameraShift () macro
* SYNOPSIS
IntelligentEyeT (lam, phi, r)
* INPUTS
*  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
*                 possibly set/reset by SetCameraTarget () macro
* SEE ALSO
*  SetCameraAngle ()
*  SetCameraShift ()
*  IntelligentEye ()
****/
#macro IntelligentEyeT(lam, phi, r)
// lam : longitude
// phi : latitude
// r   : distance from XYZ origin
#warning ">> CameraShift assumed in REAL coordinates! <<"
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam);
  #declare  camera_y = r*sin(phi);
  #declare  camera_z =-r*cos(phi)*cos(lam);
  #declare  Xcent = (SceneMinX+SceneMaxX)/2+CameraShift.y;  // hypothetic center of entire scene plus Shift in REAL coorfdinates
  #declare  Ycent = (SceneMinY+SceneMaxY)/2+CameraShift.z;  // hypothetic center of entire scene plus Shift in REAL coorfdinates
  #declare  Zcent = (SceneMinZ+SceneMaxZ)/2-CameraShift.x;  // hypothetic center of entire scene plus Shift in REAL coorfdinates

// If SetTarget was called explicitly before, this call is inactive, and 'CameraTarget' remains unchanged
  #ifndef(CameraTarget_SET)
    SetCameraTarget(Xcent, Ycent, Zcent) // point calculated above
  #end  
  
  camera{
     #if(Perspective)
         location <camera_x+CameraTarget.x+CameraShift.y,camera_y+CameraTarget.y+CameraShift.z, camera_z+CameraTarget.z-CameraShift.x> //+CameraShift
         // coordinates of the camera set above shifted to relative location
    
         right x*image_width/image_height
         up y
    
         look_at CameraTarget                                      // Camera looks at that point-set previously by SetTarget macro
         angle CameraAngle
      #else
         orthographic
         location <camera_x+CameraTarget.x,camera_y+CameraTarget.y, camera_z+CameraTarget.z>+CameraShift  //coordinates of the camera, scaled by "Dist"
         look_at CameraTarget                        // camera focuses at that point
         up OrthoScale*y
         right OrthoScale*(image_width/image_height)*x
      #end
} //camera
#end

/****iM* Cameras.inc/CameraSpher 
 * PURPOSE
 *  This Camera looks at point set by SetTarget macro
 * SYNOPSIS 
 */
#macro CameraSpher (lam, phi, r)
/*
 * INPUTS
 *  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
 *                 possibly set/reset by SetCameraTarget () macro
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 *  IntelligentEye ()
 *  IntelligentEyeT ()
 ****/
//#macro CameraSpher(lam, phi, r)
// lam : longitude
// phi : latitude
// r   : distance from XYZ origin
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam);
  #declare  camera_y = r*sin(phi);
  #declare  camera_z =-r*cos(phi)*cos(lam);
  camera{
     perspective
     location <camera_x+Xcent, camera_y+Ycent, camera_z+Zcent> // coordinates of the camera
     look_at CameraTarget                                      // Camera looks at that point-set previously by SetTarget macro
  } // camera
#end

/****M* Cameras.inc/CameraSpher0 
 * PURPOSE
 *  This Camera is placed in spherical ccordinates and looks at point set by SetCameraTarget macro. 
 * SYNOPSIS 
 */
#macro CameraSpher0 (lam, phi, r)
/*
 * INPUTS
 *  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
 *                 possibly set/reset by SetCameraTarget () macro
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 *  IntelligentEye ()
 *  IntelligentEyeT ()
 ****/
// #macro CameraSpher0(lam, phi, r)
// lam : longitude
// phi  : latitude
// r   : distance from XYZ origin
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam);
  #declare  camera_y = r*sin(phi);
  #declare  camera_z =-r*cos(phi)*cos(lam);
  camera{
     perspective
     location <camera_x,camera_y,camera_z>  // coordinates of the camera
     look_at CameraTarget                   // Camera looks at point <0,0,0> if not changed by SetCameraTarget () macro
} //camera
#end

/****M* Cameras.inc/CameraSpher1
* PURPOSE
*  This Camera is placed relative to point set by SetCameraTarget macro, and looks at it. 
* SYNOPSIS */
#macro CameraSpher1 (lam, phi, r)
/*
 * INPUTS
 *  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
 *                 possibly set/reset by SetCameraTarget () macro
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 *  IntelligentEye ()
 *  IntelligentEyeT ()
 ****/
// #macro CameraSpher1(lam, phi, r)
// lam : longitude
// phi  : latitude
// r   : distance from XYZ origin
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam)+CameraTarget.x;
  #declare  camera_y = r*sin(phi)+CameraTarget.y;
  #declare  camera_z =-r*cos(phi)*cos(lam)+CameraTarget.z;
  camera{
     perspective
     location <camera_x,camera_y,camera_z>  // coordinates of the camera
     look_at CameraTarget                   // Camera looks at point <0,0,0> if not changed by SetCameraTarget () macro
} //camera
#end


/****M* Cameras.inc/CameraSpher2
* PURPOSE
*  This Camera is placed relative to point set by SetCameraTarget macro, and looks at it. 
* SYNOPSIS */
#macro CameraSpher2 (lam, phi, r)
/*
 * INPUTS
 *  lam, phi, r -  floats: longitude[deg], latitude[deg], distance from actual watch point [scene units] 
 *                 possibly set/reset by SetCameraTarget () macro, keeps moving toward object if "r" is negative, 
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 *  IntelligentEye ()
 *  IntelligentEyeT ()
 ****/
// #macro CameraSpher1(lam, phi, r)
// lam : longitude
// phi  : latitude
// r   : distance from XYZ origin
  #declare  lam = radians(lam);
  #declare  phi = radians(phi);
  #declare  camera_x = r*cos(phi)*sin(lam)+CameraTarget.x;
  #declare  camera_y = r*sin(phi)+CameraTarget.y;
  #declare  camera_z =-r*cos(phi)*cos(lam)+CameraTarget.z;                                   
  
  
  camera{
     perspective
     location <camera_x,camera_y,camera_z>  // coordinates of the camera
     #if (r>0)
        look_at CameraTarget
     #else
        look_at CameraTarget + 2*(CameraTarget-<camera_x,camera_y,camera_z>)  // Camera looks at point <0,0,0> if not changed by SetCameraTarget () macro
     #end
} //camera
#end



/****M* Cameras.inc/CameraLights
* PURPOSE
*  Addon lights (left&right) connected with camera placed next to it oriented on point defined in SetCameraTarget macro.
* SYNOPSIS 
*/
#macro CameraLights (_Dist, _sep, _col, _fan, _rd) 
/*
 * INPUTS                   
 *  _Dist- target-light distance (may be different from camera-target)
 *  _sep - separation between lights
 *  _col - light color
 *  _fan - falloff angle 
 *  _rd  - halftone radius 
 * SEE ALSO
 *  SetCameraAngle ()
 *  SetCameraShift ()
 *  IntelligentEye ()
 *  IntelligentEyeT ()
 ****/
  #ifdef(CameraTarget_SET)
      #declare Xcent=CameraTarget.x;
      #declare Ycent=CameraTarget.y;
      #declare Zcent=CameraTarget.z;
  #else
      #declare Xcent=(SceneMinX+SceneMaxX)/2;  // hypothetic center of entire chart
      #declare Ycent=(SceneMinY+SceneMaxY)/2;
      #declare Zcent=(SceneMinZ+SceneMaxZ)/2;
  #end

  #local _C = vnormalize(<camera_x, camera_y, camera_z>); 
  #local _L = _Dist*_C + <Xcent,Ycent, Zcent>;     // new shifted location of lights
  #local _D = _sep*vnormalize(vcross(CameraTarget-<camera_x,camera_y,camera_z> , y  ));
  
  light_source {
         _L + _D, _col  // coordinates of the camera biased by CameraShift
         spotlight 
         radius    _rd
         falloff  _fan
         tightness  0
         point_at CameraTarget
  }//camera 1
 
  light_source {
         _L - _D, _col  // coordinates of the camera biased by CameraShift
         spotlight 
         radius   _rd
         falloff _fan
         tightness  0
         point_at CameraTarget
  }//camera 2
#end


/****M* Cameras.inc/InsertCylKeyItem
* PURPOSE
*  Inserts legend key with right hand extended text description. Uses cylinder as legend key.
*  MUST be inserted just after using SetRGBMap () macro, 
*  as it purposely utilizes internal variable _RGBMap to keep scene consistent!
*  As it uses Camera location data it must be used AFTER PrepareCamera () macro!
* SYNOPSIS
*/
#macro InsertCylKeyItem ( _Ts, _Ttext, _scf, _Krad, _vf ) // description, descr. texture, scal.factor, band wsdth, band. texture, position
/*
* INPUTS
*  text - description, 
*  texture - description texture, 
*  float - text scaling factor, 
*  float - legend key width, 
*  float - position counted from bottom (not percent of image height by NOW)
* OUTPUTS
*  Legend key as POVRay object                                           
* EXAMPLE
*  SetRGBMap(RGB5)
*  InsertCylKeyItem ( "V(r)", txMica, .09, .04, .054) 
* SEE ALSO
*  InsertBarKeyItem 
*
******/

#local VC  = <camera_x, camera_y, camera_z>;   // where is Camera?
#local VT  = CameraTarget;         // where is target?
#local VCT = vnormalize(VT-VC);    // normal vector from camera toward target
#local VP  = vcross(y, VCT);       // horizontal r-hand vector   
#if (abs(the_)<89.99)
  #local VP  = vnormalize(<VCT.z ,0, -VCT.x >);
  #local VUD = vnormalize(vcross(VP, VCT));      // Up-Down vector
#else            
  #local VP  = x;
  #local VUD = -z;
#end  

#local V3 = VC + 5*vnormalize(VCT+_vf*VUD); // in front of Camera
union{
    cylinder{ <.0, 0.0001, .0>, 
              <.0,  .9999, .0>,
              _Krad // *** // ***, 1, .2
        texture{ 
                  pigment{ 
                    gradient y
                    color_map{_RGBMap}
                  }                               
                  finish{ scDullMirror emission 1 }
        }     
        scale <1,2,1>        
        translate <0,-1,0>
        rotate 90*x                       
        #if (abs(the_)<89.99)
            rotate (90-alf_)*y
        #else
            rotate 90*y
        #end    
    }     

    #ifdef (_Ts)
        #local _txt  = object{Text(_Ts)};
        #local tMin  = min_extent(_txt);   
        #local tMax  = max_extent(_txt);
        #local _wdth = tMax.x; // text object far end
        #local _hght = tMax.y; // text object height
            
        #debug concat("Text wdth=", str(_wdth,10,5), "\n")
        object {
          _txt
          texture{ _Ttext finish{ emission 1}} // T_Copper_3C | txMica
          scale _scf*<1,1,1>     
          #local _wdth = _wdth*_scf;
          #local _hght = _hght*_scf;           
          translate -_hght*y/2 
          rotate -90*y   
          rotate (90-the_)*z
          translate 1.02*z // (.98 - _wdth)*VP          
          #if (abs(the_)<89.99)
            rotate (90-alf_)*y
          #else
            rotate 90*y
          #end    
        }         
    #end
    translate V3
}
#end

/****M* Cameras.inc/InsertBarKeyItem
* PURPOSE
*  Inserts legend key with right hand extended text description. Uses perpendicular prism as legend key. 
*  MUST be inserted just after using SetRGBMap () macro, 
*  as it purposely utilizes internal variable _RGBMap to keep scene consistent!
*  As it uses Camera location data it must be used AFTER PrepareCamera () macro!
* SYNOPSIS
*/
#macro InsertBarKeyItem ( _Ts, _Ttext, _scf, _Krad, _vf ) // description, descr. texture, scal.factor, band wsdth, band. texture, position
/*
* INPUTS
*  text - description, 
*  texture - description texture, 
*  float - text scaling factor, 
*  float - legend key width, 
*  float - position counted from bottom (not percent of image height by NOW)
* OUTPUTS
*  Legend key as POVRay object                                           
* EXAMPLE
*  SetRGBMap(RGB5)
*  InsertBarKeyItem ( "V(r)", txMica, .09, .04, .054) 
* SEE ALSO
*  InsertCylKeyItem 
*
******/

#local VC  = <camera_x, camera_y, camera_z>;   // where is Camera?
#local VT  = CameraTarget;         // where is target?
#local VCT = vnormalize(VT-VC);    // normal vector from camera toward target
#local VP  = vcross(y, VCT);       // horizontal r-hand vector   
#if (abs(the_)<89.99)
  #local VP  = vnormalize(<VCT.z ,0, -VCT.x >);
  #local VUD = vnormalize(vcross(VP, VCT));      // Up-Down vector
#else            
  #local VP  =  x;
  #local VUD = -z;
#end  

#local V3 = VC + 5*vnormalize(VCT+_vf*VUD); // in front of Camera
union{
    box{ < -.01, 0, -_Krad/2>, 
         <  .00, 1,  _Krad/2>
        texture{ 
                  pigment{ 
                    gradient y
                    color_map{_RGBMap}
                  }                               
                  finish{ scDullMirror emission 1 }
        }     
        scale <1,2,1>        
        translate <0,-1,0>
        rotate 90*x                       
    }     

    #ifdef (_Ts)
        #local _txt  = object{Text(_Ts)};
        #local tMin  = min_extent(_txt);   
        #local tMax  = max_extent(_txt);
        #local _wdth = tMax.x; // text object far end
        #local _hght = tMax.y; // text object height
            
        #debug concat("Text wdth=", str(_wdth,10,5), "\n")
        object {
          _txt
          texture{ _Ttext finish{ emission 1}} // T_Copper_3C | txMica
          scale _scf*<1,1,1>     
          #local _wdth = _wdth*_scf;
          #local _hght = _hght*_scf;           
          translate -_hght*y/2 
          rotate -90*y   
          translate 1.02*z // (.98 - _wdth)*VP          
        }         
    #end
    rotate the_*z

    #if (abs(the_)<89.99)
        rotate (90-alf_)*y
    #else
        rotate 90*y
    #end    

    translate V3
}
#end


/****M* Cameras.inc/InsertBarKeyFrItemBelow
* PURPOSE
*  Inserts legend key with extended text description BELOW, centered. Uses perpendicular prism as legend key. 
*  MUST be inserted just after using SetRGBMap () macro, 
*  as it purposely utilizes internal variable _RGBMap to keep scene consistent!
*  As it uses Camera location data it must be used AFTER PrepareCamera () macro!
* SYNOPSIS
*/
#macro InsertBarKeyFrItemBelow ( _Ts, _Ttext, _scf, _Krad, _vf, _frTxt, _frd ) // description, descr. texture, scal.factor, band wsdth, band. texture, position
/*
* INPUTS
*  text - description, 
*  texture - description texture, 
*  float - text scaling factor, 
*  float - legend key width, 
*  float - position counted from bottom (not percent of image height by NOW)
*  texture - "frame" texture
*  float - width of "frame" 
* OUTPUTS
*  Legend key as POVRay object                                           
* EXAMPLE
*  SetRGBMap(RGB5)
*  InsertBarKeyFrItemBelow ( "V(r)", txYellow, .09, .04, .054, txMica, .02) 
* SEE ALSO
*  InsertCylKeyItem 
*
******/

#local VC  = <camera_x, camera_y, camera_z>;   // where is Camera?
#local VT  = CameraTarget;         // where is target?
#local VCT = vnormalize(VT-VC);    // normal vector from camera toward target
#local VP  = vcross(y, VCT);       // horizontal r-hand vector   
#if (abs(the_)<89.99)
  #local VP  = vnormalize(<VCT.z ,0, -VCT.x >);
  #local VUD = vnormalize(vcross(VP, VCT));      // Up-Down vector
#else            
  #local VP  =  x;
  #local VUD = -z;
#end  

light_source{VC color rgb 1.5}

#ifdef (_Ts)
    #local _txt   = object{Text(_Ts)};
    #local _tMin  = min_extent(_txt);   
    #local _tMax  = max_extent(_txt);
    #local _twdth = _scf*(_tMax.x-_tMin.x); // text object far end
    #local _wdth  = (_twdth<_LabelWidth?_LabelWidth:_twdth);
    #local _hght  = _scf*(_tMax.y-_tMin.y); // text object height
    #local _dpth  = _scf*abs((_tMax.z-_tMin.z));
// only number's correction - text object still needs real scaling
    #if (Debug>0)
        DebugMsg(concat("Text width =", str(_wdth,10,5), "\n"), 2)
        DebugMsg(concat("Text height=", str(_hght,10,5), "\n"), 2)
        DebugMsg(concat("Text depth =", str(_dpth,10,5), "\n"), 2)
    #end    
#end

#local V3 = VC + 5*vnormalize(VCT+_vf*VUD); // in front of Camera
merge{
    union{ ///???? Xor merge{
//        box{ < -.01, -_wdth/2, -_Krad/2>, 
//             <  .00,  _wdth/2,  _Krad/2>
        box{ < -.01, 0, -_Krad/2>, 
             <  .00, 1,  _Krad/2>

            texture{ 
                      pigment{ 
                        gradient y
                        color_map{_RGBMap}
                      }                               
                      finish{ scDullMirror emission 1 }
            }                
            scale<1,_wdth,1>
            translate<0,-_wdth/2,0>
        }         
        box{ < -.01, -_wdth/2-_frd, -_Krad/2-_frd>, // corr. 5 IV 2018
             < -.009, _wdth/2+_frd,  _Krad/2+_frd+1.4*_hght>//+6*_frd>
            texture{ _frTxt finish{ Dull emission 1 }}     
        }
        rotate 90*x                       
    }
    #ifdef (_txt)
        object {
          _txt
          texture{ _Ttext finish{ emission 1}} // T_Copper_3C | txMica
          scale _scf*<1,1,1>     
          translate <-_twdth/2, -(_Krad/2+_frd+_hght), -.7*_dpth>
          rotate -90*y   
        }         
    #end

    rotate the_*z
    #if (abs(the_)<89.99)
      rotate (90-alf_)*y
    #else
      rotate 90*y
    #end    
    translate V3
}
#end

/****M* Cameras.inc/InsertBarKeyFrItem
* PURPOSE
*  Inserts legend key with right hand extended text description. Uses perpendicular prism as legend key. 
*  MUST be inserted just after using SetRGBMap () macro, 
*  as it purposely utilizes internal variable _RGBMap to keep scene consistent!
*  As it uses Camera location data it must be used AFTER PrepareCamera () macro!
* SYNOPSIS
*/
#macro InsertBarKeyFrItem ( _Ts, _Ttext, _scf, _Krad, _vf, _frd ) // description, descr. texture, scal.factor, band wsdth, band. texture, position
/*
* INPUTS
*  text - description, 
*  texture - description texture, 
*  float - text scaling factor, 
*  float - legend key width, 
*  float - position counted from bottom (not percent of image height by NOW)
* OUTPUTS
*  Legend key as POVRay object                                           
* EXAMPLE
*  SetRGBMap(RGB5)
*  InsertBarKeyItem ( "V(r)", txMica, .09, .04, .054) 
* SEE ALSO
*  InsertCylKeyItem 
*
******/

#local VC  = <camera_x, camera_y, camera_z>;   // where is Camera?
#local VT  = CameraTarget;         // where is target?
#local VCT = vnormalize(VT-VC);    // normal vector from camera toward target
#local VP  = vcross(y, VCT);       // horizontal r-hand vector   
#if (abs(the_)<89.99)
  #local VP  = vnormalize(<VCT.z ,0, -VCT.x >);
  #local VUD = vnormalize(vcross(VP, VCT));      // Up-Down vector
#else            
  #local VP  =  x;
  #local VUD = -z;
#end  

light_source{VC color rgb .5}

#local V3 = VC + 5*vnormalize(VCT+_vf*VUD); // in front of Camera
union{
    merge{
        box{ < -.01, 0, -_Krad/2>, 
             <  .00, 1,  _Krad/2>
            texture{ 
                      pigment{ 
                        gradient y
                        color_map{_RGBMap}
                      }                               
                      finish{ scDullMirror emission 1 }
            }
            scale <1,2,1>        
            translate <0,-1,0>
        }         
        box{ < -.01, -1.0-_frd, -_Krad/2-_frd>, 
             < -.009, 1.0+_frd,  _Krad/2+_frd>
            texture{ pigment{color rgb 1} finish{ Dull emission 1 }}     
        }
        rotate 90*x                       
    }     

    #ifdef (_Ts)
        #local _txt  = object{Text(_Ts)};
        #local tMin  = min_extent(_txt);   
        #local tMax  = max_extent(_txt);
        #local _wdth = tMax.x; // text object far end
        #local _hght = tMax.y; // text object height
            
        #debug concat("Text wdth=", str(_wdth,10,5), "\n")
        object {
          _txt
          texture{ _Ttext finish{ emission 1}} // T_Copper_3C | txMica
          scale _scf*<1,1,1>     
          #local _wdth = _wdth*_scf;
          #local _hght = _hght*_scf;           
          translate -_hght*y/2 
          rotate -90*y   
          translate 1.02*z // (.98 - _wdth)*VP          
        }         
    #end
    rotate the_*z

    #if (abs(the_)<89.99)
        rotate (90-alf_)*y
    #else
        rotate 90*y
    #end    
    translate V3
}
#end

/****M* Cameras.inc/BirdsEye 
* PURPOSE
*  Bird eye camera.
*  Gets position based on given spline function _Spl and vector parameter _s 
*  Bird looks down his trajectory. (NOW)
* SYNOPSIS      
*/
#macro BirdsEye ( _dst, _s )
/* 
*  INPUTS
*  _Spl - spline function - indirect
*  _dst - distance parameter       
*  _s   - camera position on the path 
* SEE ALSO
*  setPerspective ()
*  SetOrthographic ()
*  SetCameraTarget ()
****/   

  #declare _CameraVec   = _Spl(_s);     
  #declare _CameraVecAt = _Spl(clip(_s+.001,0,1));       
  
  #local _spl    = clip(_s+0.01, 0.01, 1.00);  
  #local _nplus  = _Spl(_spl);  
               
  #local _smi    = clip(_s-0.01, 0.00, 0.99);
  #local _nminus = _Spl(_smi);

  #local _nvec = vnormalize(_nplus  - _nminus);
  #local _rgh  = vnormalize(vcross(_nvec, y));
  #local _upp  = vnormalize(vcross(_rgh, _nvec));
  #local _nvec = vaxis_rotate(_nvec,_rgh,-30);
  
  #debug concat(str(_spl,9,6), ", n+ = ", Vstr(_nplus, 9,6), " |",str(_smi,9,6)," n- =", Vstr( _nminus, 9,6), "\n")    

  #declare camera_x = _CameraVec.x + CameraShift.y; // CameraShift is given in REAL coords.
  #declare camera_y = _CameraVec.y + CameraShift.z;
  #declare camera_z = _CameraVec.z - CameraShift.x; 
  
  #declare Xcent = camera_x+_dst*_nvec.x;
  #declare Ycent = camera_y+_dst*_nvec.y;
  #declare Zcent = camera_z+_dst*_nvec.z;

  #declare angiel = 1.4*max(Xcent, Ycent, Zcent)/VDist(<camera_x, camera_y, camera_z>, <Xcent, Ycent, Zcent>);
//                   1.4*tangens of angle between camera and most distant chart point

#debug concat("right = ", Vstr(_rgh, 9,6), " | up=", Vstr(_upp, 9,6), "\n")    
  camera{
     #if(Perspective)
         fisheye //perspective
         location <camera_x, camera_y, camera_z>  // coordinates of the camera biased by CameraShift
         up y
         //right image_width/image_height*x   
         right x/1. // _rgh             
         look_at <Xcent, Ycent, Zcent>            // camera focuses at that point
         
         translate -<camera_x, camera_y, camera_z>
         #local _roll=0.0;
//         #local _roll=360*clock;
//         rotate _roll*_nvec
//         #local _rgh = vaxis_rotate(_rgh, _nvec, _roll);
         rotate 20*_rgh
         translate <camera_x, camera_y, camera_z>
         
/*
         location <camera_x, camera_y, camera_z>  // coordinates of the camera biased by CameraShift
         up y    // _upp
         // right image_width/image_height*x      // _rgh
         right x/1. // _rgh    
         look_at <Xcent, Ycent, Zcent>            // camera focuses at that point
*/         
         
         #if (CameraAngleSet)
            angle CameraAngle
         #else
            angle clip(atan(angiel)*180/pi, 10, 170)
         #end           
         
         //rotate 30*x
         #if (Debug>0) #debug concat("\nView angle= ", str( clip(atan(angiel)*180/pi, 10, 170), 10, 6 ), "\n") #end
     #else
         orthographic
         location <camera_x, camera_y, camera_z>  // coordinates of the camera, scaled by "Dist"
         look_at <Xcent, Ycent, Zcent>            // camera focuses at that point
         up OrthoScale*y
         right OrthoScale*(image_width/image_height)*x
     #end
} //camera
#end

// This camera looks at once established point "_AAt" from location "_Loc"
/****M* Cameras.inc/FCamera 
* PURPOSE
*  Gets position _Loc and target point _AAt
*  tilts by _T degrees, zooms by _Zoom, in or out 
* SYNOPSIS      
*/
#macro FCamera( _Loc  // camera location
              , _AAt  // aim camera at this point
              , _Zoom // Zoom xD
              )
/* 
*  INPUTS
*  _Loc  - camera location in povray coords
*  _AAt  - aim camera at this 3D povray point (really!)       
*  _Zoom - Just zoom: >1 - zoom in, <1 - zoom out, negative values aren't interpreted 
* SEE ALSO
*  none
****/   
 camera {         
   #local _d = vnormalize(_AAt - _Loc);
   location  _Loc 
   right     x*image_width/image_height
   look_at _AAt  
   direction abs(_Zoom)*_d
 }
#end                 

// This camera looks at once established point "_AAt" from location "_Loc"
// added tilt (or bank) in degrees [-180;180] cyclic (to the left is 'minus')
/****M* Cameras.inc/FTiltCamera 
* PURPOSE
*  Tilted camera.
*  Gets position _Loc and target point _AAt
*  tilts by _T degrees, zooms by _Zoom, in or out 
* SYNOPSIS      
*/
#macro FTiltCamera( _Loc    // camera location
                  , _AAt    // aim camera at this point
                  , _Zoom   // Zoom xD
                  , _T      // camera tilt (or bank) in degrees (to the left 'minus')
                  )
/* 
*  INPUTS
*  _Loc  - camera location in povray coords
*  _AAt  - aim camera at this 3D povray point (really!)       
*  _Zoom - Just zoom: >1 - zoom in, <1 - zoom out, negative values aren't interpreted 
*  _T    - tilt or bank in degrees, minus left, positive right.
* SEE ALSO
*  none
****/   
 camera {         
   #local _d = vnormalize(_AAt - _Loc);
   location  _Loc 
   right     x*image_width/image_height
   #if (vlength(vcross(y,_d))>0)
     #local _xrgh = vnormalize(vcross(y, _d));
   #else #local _xrgh = x;
   #end  
   sky vaxis_rotate((vcross(_d, _xrgh)), _d, _T)
    
   look_at _AAt   
   direction abs(_Zoom)*_d
 }
#end                 
         
#end // Cameras_INC


//**************[ obsolete ]******************************
//#macro Kamerka(lam, phi, r)
// lam : longitude
// phi  : latitude
// r   : distance from XYZ origin
//  #declare  lam = radians(lam);
//  #declare  phi = radians(phi);
//  #declare  camera_x = r*cos(phi)*sin(lam);
// #declare  camera_y = r*sin(phi);
//  #declare  camera_z =-r*cos(phi)*cos(lam);
//  camera{
//     perspective
//     location <camera_x,camera_y,camera_z>  // coordinates of the camera
//     look_at CameraTarget                   // Camera looks at point <0,0,0> if not changed by SetCameraTarget () macro
//} //camera
//#end
//********************************************/





