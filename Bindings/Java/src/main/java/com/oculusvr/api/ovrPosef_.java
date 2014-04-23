package com.oculusvr.api;
import com.sun.jna.Structure;
import java.util.Arrays;
import java.util.List;
/**
 * <i>native declaration : OVR_CAPI.h</i><br>
 * This file was autogenerated by <a href="http://jnaerator.googlecode.com/">JNAerator</a>,<br>
 * a tool written by <a href="http://ochafik.com/">Olivier Chafik</a> that <a href="http://code.google.com/p/jnaerator/wiki/CreditsAndLicense">uses a few opensource projects.</a>.<br>
 * For help, please visit <a href="http://nativelibs4java.googlecode.com/">NativeLibs4Java</a> , <a href="http://rococoa.dev.java.net/">Rococoa</a>, or <a href="http://jna.dev.java.net/">JNA</a>.
 */
public class ovrPosef_ extends Structure {
  /** C type : ovrQuatf */
  public ovrQuatf_ Orientation;
  /** C type : ovrVector3f */
  public ovrVector3f_ Position;
  public ovrPosef_() {
    super();
  }
  protected List<? > getFieldOrder() {
    return Arrays.asList("Orientation", "Position");
  }
  /**
   * @param Orientation C type : ovrQuatf<br>
   * @param Position C type : ovrVector3f
   */
  public ovrPosef_(ovrQuatf_ Orientation, ovrVector3f_ Position) {
    super();
    this.Orientation = Orientation;
    this.Position = Position;
  }
  public static class ByReference extends ovrPosef_ implements Structure.ByReference {
    
  };
  public static class ByValue extends ovrPosef_ implements Structure.ByValue {
    
  };
}