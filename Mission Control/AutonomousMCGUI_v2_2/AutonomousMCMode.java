package v2_2;

import javafx.event.ActionEvent;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

public class AutonomousMCMode extends VBox {

	String[] modes = {"0- Disabled", "1- RC", "2- Train", "3- Time Trial", "4- Race"};
	Label commandModeLabel, actualModeLabel;
	double commandMode;
	boolean eStopped;
	
	public AutonomousMCMode() {
		super(5);
		this.setAlignment(Pos.CENTER);
		
		eStopped = false;
		
		//Title
		Label title = new Label(" Mode ");
		title.getStyleClass().add("modeTitle");
		this.getChildren().add(title);
		
		HBox contents = new HBox(15);
		
		//Left Button
		Button leftButton = new Button("<");
		leftButton.setOnAction((ActionEvent e) -> {
			if(!eStopped) {
				if(commandMode > 1) {
					commandMode--;
				}
				commandModeLabel.setText("Command: " + String.format("%-13s", modes[(int)commandMode]) );
			}
		});
		leftButton.setMinHeight(50);
		contents.getChildren().add(leftButton);
		
		//Display
		VBox labels = new VBox(5);
		
		commandMode = 0;
		commandModeLabel = new Label("Command: " +  String.format("%-13s", modes[(int)commandMode]) );
		commandModeLabel.getStyleClass().add("modeLabel");
		labels.getChildren().add(commandModeLabel);
		
		actualModeLabel = new Label("Actual: " + modes[0]);
		actualModeLabel.getStyleClass().add("modeLabel");
		labels.getChildren().add(actualModeLabel);
		
		contents.getChildren().add(labels);
		
		//Right Button
		Button rightButton = new Button(">");
		rightButton.setOnAction((ActionEvent e) -> {
			if(!eStopped) {
				if(commandMode < 4) {
					commandMode++;
				}
				commandModeLabel.setText("Command: " + String.format("%-13s", modes[(int)commandMode]) );
			}
		});
		rightButton.setMinHeight(50);
		contents.getChildren().add(rightButton);
		
		this.getChildren().add(contents);
	}
	
	public void setActualValue(double value) {
		actualModeLabel.setText("Actual:  " + String.format("%-13s", modes[(int)value] ));
	}
	
	public double getCommandMode() {
		return commandMode;
	}
	
	public void sendEStop() {
		eStopped = true;
		commandMode = 1;
		commandModeLabel.setText("Command: " + String.format("%-13s", modes[0]) );
	}
	
	public void revokeEStop() {
		eStopped = false;
		commandModeLabel.setText("Command: " + String.format("%-13s", modes[(int)commandMode]) );
	}
	
}
