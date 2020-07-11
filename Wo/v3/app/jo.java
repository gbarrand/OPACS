//
// Usage :
// ( csh> setenv WOENVIRONMENT WoTest.odb )
//  UNIX> java jo
//
class jo {
  public 
  static void main(String[] args) {
    Wo                  wo;
    wo                  = new Wo();  
    wo.SetInterfaceFile ("WoTest.odb");
    wo.Startup          ();
    wo.ProcessEvents    ();
    wo.ClearClass       ();
  }
}

