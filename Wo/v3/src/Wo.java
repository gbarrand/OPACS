class Wo
{
  public native void Startup          ();
  public native void ProcessEvents    ();
  public native void ClearClass       ();
  public native void SetInterfaceFile (String fileName);
  Wo ()
  {
    System.loadLibrary ("WoJAVA");
  }
}

