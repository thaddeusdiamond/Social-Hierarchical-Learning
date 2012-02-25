import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import java.io.*;
import java.net.*;

/**
 * @class FeedbackController
 * @brief Implementation of the Experiment Controller for subjects
 * 
 * This controller presents various buttons based on predefined
 * globals and allows the user to press them, while transmitting
 * the results of the clicks over UDP
 **/
class FeedbackController {
  /**
   * MODIFY THESE VARIABLES to contain the proper host name and port
   * that the information should be sent to
   **/
  public static final String HOSTNAME = "localhost";
  public static final int HOSTPORT = 20000;
  
  /**
   * MODIFY THESE VARIABLES to affect the presentation of the GUI (height, width,
   * how many buttons per row, and the padding at the end
   **/
  private final int SCREEN_WIDTH = 800;
  private final int SCREEN_HEIGHT = 600;
  private final int NUMBER_PER_ROW = 2;
  private final int BUTTON_PADDING = 11;
  
  /**
   * MODIFY THIS VARIABLE to control what button options are presented to the user
   **/
  private final String button_labels[] = new String[] { 
    "Good", "Bad", "Panic", "Done"
  };
  
  /**
   * MODIFY THIS VARIABLE to control the colors of the buttons presented to the user
   **/
  private final Color button_colors[] = new Color[] {
    Color.WHITE, Color.BLACK, Color.RED, Color.GREEN
  };
  
  /**
   * Entry point for GUI to run, notice thread safety
   **/
  public static void main(String[] args) {
    javax.swing.SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        FeedbackController controller = new FeedbackController(HOSTNAME, HOSTPORT);
        controller.LaunchController();
      }
    });
  }
  
  /**
   * Each controller sends message over a socket...
   **/
  private DatagramSocket socket;

  /**
   * ... to a recipient at a specified IPAddress...
   **/
  private InetAddress recipient;
  
  /**
   * ... and port.
   **/
  private int port;
  
  /**
   * A feedback controller initializes the connection to the remote data collection
   * source if possible
   * 
   * @param      hostname       The host to connect to
   * @param      port           The port on the host to connect to
   **/
  public FeedbackController(String hostname, int port) {
    try {
      this.recipient = InetAddress.getByName(hostname);
      this.port = port;
      this.socket = new DatagramSocket();
    } catch (Exception e) {
      e.printStackTrace();
      System.out.println("Boo... couldn't connect on socket");
    }
  }
  
  /**
   * The LaunchController() method initializes the entire GUI framework and makes
   * it visible.
   **/
  public void LaunchController() {
    // JFrames need a special JPanel for multiple buttons
    JFrame frame = new JFrame("Feedback Controller");
    frame.setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
    JPanel panel = new JPanel();
    
    // Initialize all the buttons individually
    for (int i = 0; i < button_labels.length; i++) {
      JButton button = new JButton();
      button.setSize(SCREEN_WIDTH / NUMBER_PER_ROW, SCREEN_HEIGHT / button_labels.length * NUMBER_PER_ROW);
      button.setLocation(new Point(SCREEN_WIDTH / NUMBER_PER_ROW * (i % NUMBER_PER_ROW),
                                   SCREEN_HEIGHT / button_labels.length * NUMBER_PER_ROW * (i / NUMBER_PER_ROW)));
      button.setOpaque(true);
      button.setBackground(button_colors[i]);
      button.setFont(new Font("Helvetica", Font.PLAIN, 60));
      
      button.setAction(new FeedbackController.ButtonClickAction(button_labels[i], i));
      panel.add(button, BorderLayout.CENTER);
    }
    
    // Render (NOTE: We add a little height due to shading on the buttons)
    panel.setLayout(null);
    frame.setMinimumSize(new Dimension(SCREEN_WIDTH, 
                                       SCREEN_HEIGHT + (BUTTON_PADDING * button_labels.length / NUMBER_PER_ROW)));
    frame.setContentPane(panel);
    frame.setVisible(true);
  }
  
  /**
   * @class ButtonClickAction
   * @brief Button handler for simple GUI
   * 
   * This is a simple action that sends the clicked button over UDP and prints
   * it out to the command line
   **/
  private class ButtonClickAction extends AbstractAction {
    private int status;
    
    /**
     * The constructor takes a string of text to display on the button and its
     * corresponding status
     * 
     * @param      text     The text to appear on the button
     * @param      status   The status code to send over UDP when clicked
     **/
    public ButtonClickAction(String text, int status) {
      super(text);
      this.status = status;
    }
    
    /**
     * Override the standard action handler to send out this action's status
     **/
    @Override
    public void actionPerformed(ActionEvent e) {
      System.out.println("Sending: " + status + " (" + button_labels[status] + ")");
      try {
        String message = "" + status;
        socket.send(new DatagramPacket(message.getBytes(), message.length(), recipient, port));
      } catch (Exception error) {
        error.printStackTrace();
        System.out.println("Failed to send status: " + status);
      }
    }
  }
}
