/************
 * Purdue evGrandPrix Autonomous Team
 * Mission Control Software v2.0.0
 * 
 * Authors: Tyler Mahlmann
 * Date: Feb 2019
 * 
 * Displays Kart diagnostics and telemetry while kart is untethered
 * 
 * Class Descriptions:
 * Main- Sets the Stage and launches the Menu scene
 * 
 * AutonomousMCMenu- Creates the Menu scene.
 * Menu scene lets user configure options for the GUI
 * Once configured, creates Serial and GUI scene
 * 
 * AutonomousMCSerial- Handles RFT and FPV communication
 * via 2 USB ports. Configured in Menu
 * 
 * AutonomousMCGUI- The central scene of the program.
 * Creates and updates all visual elements.
 * 
 * AutonomousMCDial- Dial GUI element
 * 
 * AutonomousMCSteering- Steering GUI element
 * 
 * AutonomousMCIMU- IMU GUI element
 * 
 * AutonomousMCFiles- Handles file writing to host machine. 
 * 
 ************/

package v2_1;

import javafx.application.Application;
import javafx.stage.Stage;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.layout.GridPane;

public class Main_2_1 extends Application {
	
	final short RESOLUTION_X = 1280;
	final short RESOLUTION_Y = 720;
	
	@Override
	public void start(Stage primaryStage) {
		try {	
			//System.out.println(javafx.scene.text.Font.getFamilies()); //See all fonts
			GridPane menuRootPane = new GridPane();
			
			new AutonomousMCMenu(primaryStage, menuRootPane);
			
			Scene menuScene = new Scene(menuRootPane, RESOLUTION_X, RESOLUTION_Y);
			menuScene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());
			
			Image applicationIcon = new Image(getClass().getResourceAsStream("/resources/Autobot.jpg"));
			primaryStage.getIcons().add(applicationIcon);

			primaryStage.setTitle("Autonomous Kart Mission Control v2.1");
			primaryStage.setScene(menuScene);
			primaryStage.show();
		} catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	public void stop() {
		//Stop all threads. This one first!
		AutonomousMCSerial.stopThreads();
		
		//Close serial ports
		AutonomousMCSerial.closeAllPorts();
		
		//Save and close file
		AutonomousMCFiles.closeFiles();	
	}
	
	public static void main(String[] args) {
		launch(args);
	}
}