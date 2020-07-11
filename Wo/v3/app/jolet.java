//
// Usage :
//  UNIX> cd .../Wo/v3/usr
//   csh> setenv WOENVIRONMENT WoTest.odb
//  UNIX> appletviewer jolet.htm
//
import java.applet.Applet;

public class jolet extends Applet
{
  public synchronized void init()
    {
      Wo wo;
      wo = new Wo();
      wo.Startup ();
      wo.ProcessEvents ();
      wo.ClearClass ();
    }
}



