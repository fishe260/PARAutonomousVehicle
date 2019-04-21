package v2_1;

import javafx.event.ActionEvent;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

public class AutonomousMCMenu {

	private Stage ourStage;
	
	public AutonomousMCMenu(Stage primaryStage, GridPane menuPane) {
		ourStage = primaryStage;
		
		menuPane.getStyleClass().add("menuScene");
		menuPane.setAlignment(Pos.CENTER);
		menuPane.setHgap(10);
		menuPane.setVgap(10);
		menuPane.setPadding(new Insets(100, 100, 100, 100));
		menuPane.setGridLinesVisible(true);
		
		Button startButton = new Button("Start");
		startButton.getStyleClass().add("menuStartBtn");
		startButton.setOnAction((ActionEvent e) -> {
			changeScene();
		});
		
		//startButton.setMinWidth(value);
		
		menuPane.add(startButton, 0, 0, 3, 1);
	}
	
	public void changeScene() {
		//GUI Pane
		AnchorPane guiRoot = new AnchorPane();
		new AutonomousMCGUI(guiRoot);	
		guiRoot.setManaged(false);
		
		//Serial Thread
		Thread serialThread = new Thread(new AutonomousMCSerial()); 
		serialThread.setPriority(Thread.MIN_PRIORITY);
		serialThread.start();
		
		//GUI Scene
		Scene guiScene = new Scene(guiRoot, ourStage.getWidth(), ourStage.getHeight());
		guiScene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());
		
		ourStage.setScene(guiScene);
	}

}