package v2_0;

import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.event.ActionEvent;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.GridPane;
import javafx.stage.Stage;

public class AutonomousMCMenu {

	private Stage ourStage;
	boolean RFTActive;
	boolean FPVActive;
	
	public AutonomousMCMenu(Stage primaryStage, GridPane menuPane) {
		ourStage = primaryStage;
		
		menuPane.getStyleClass().add("menuScene");
		menuPane.setAlignment(Pos.CENTER);
		menuPane.setHgap(10);
		menuPane.setVgap(10);
		menuPane.setPadding(new Insets(100, 100, 100, 100));
		//menuPane.setGridLinesVisible(true);
		
		Label optionsLabel = new Label("Select Options:");
		optionsLabel.getStyleClass().add("menuLabel");
		
		CheckBox rftCheckBox = new CheckBox("Telemetry");
		rftCheckBox.selectedProperty().addListener(new ChangeListener<Boolean>() {
			@Override
			public void changed(ObservableValue<? extends Boolean> ob, Boolean oldVal, Boolean newVal) {
				RFTActive = newVal;
			}
		});
		
		CheckBox fpvCheckBox = new CheckBox("FPV Camera");
		fpvCheckBox.setDisable(true); //While feature unavailable
		fpvCheckBox.selectedProperty().addListener(new ChangeListener<Boolean>() {
			@Override
			public void changed(ObservableValue<? extends Boolean> ob, Boolean oldVal, Boolean newVal) {
				FPVActive = newVal;
			}
		});
		
		Button startButton = new Button("sTart");
		startButton.getStyleClass().add("menuStartBtn");
		startButton.setOnAction((ActionEvent e) -> {
			changeScene();
		});
		
		//startButton.setMinWidth(value);
		
		menuPane.add(optionsLabel, 0, 1);
		menuPane.add(rftCheckBox, 0, 2);
		menuPane.add(fpvCheckBox, 5, 2);
		menuPane.add(startButton, 0, 3);
	}
	
	public void changeScene() {
		AnchorPane guiRootPane = new AnchorPane();
		new AutonomousMCGUI(guiRootPane);	
		
		Thread serialThread = new Thread(new AutonomousMCSerial(RFTActive, FPVActive)); 
		serialThread.setPriority(Thread.MIN_PRIORITY);
		serialThread.start();

		Scene guiScene = new Scene(guiRootPane, ourStage.getWidth(), ourStage.getHeight());
		guiScene.getStylesheets().add(getClass().getResource("application.css").toExternalForm());
		
		ourStage.setScene(guiScene);
	}

}