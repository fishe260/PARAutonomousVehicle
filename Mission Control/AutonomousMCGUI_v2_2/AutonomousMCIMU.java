package v2_2;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Line;

public class AutonomousMCIMU extends StackPane{
	
	Circle icon;
	Circle background; 
	double iconXCoord;
	double iconYCoord;
	
	final static double g = 9.81; //Acceleration
	
	public AutonomousMCIMU() {
		super();
		icon = new Circle(10, Color.FIREBRICK);
		icon.setStroke(Color.BLACK);
		icon.setManaged(false); //Lets us move the circle around
		
		background = new Circle(100);
		background.setStroke(Color.BLACK);
		background.setFill(Color.color(192.0/255, 192.0/255, 192.0/255));
		this.getChildren().add(background);
		this.getChildren().add(new Line(0f, 0f, 200f, 0f));
		this.getChildren().add(new Line(0f, 0f, 0f, 200f));
		Circle innerCircle = new Circle(50, Color.TRANSPARENT);
		innerCircle.setStroke(Color.BLACK);
		this.getChildren().add(innerCircle);
		this.getChildren().add(icon);
		GridPane labelsGrid = new GridPane();
		labelsGrid.setHgap(10);
		labelsGrid.setVgap(10);
		labelsGrid.setAlignment(Pos.TOP_CENTER);
		labelsGrid.add(new Label("1G"), 0, 1);
		labelsGrid.add(new Label("0.5G"), 0, 4);
		
		this.getChildren().add(labelsGrid);
	}
	
	public void setValue(double xAccel, double yAccel) {
		//IMU range
		xAccel = AutonomousMCGUI.map(xAccel, 0, 255, -2*g, 2*g); 
		yAccel = AutonomousMCGUI.map(yAccel, 0, 255, -2*g, 2*g);
		//IMU Meter range
		iconXCoord = AutonomousMCGUI.map(xAccel, -1*g, g, 0, 2 * background.getRadius());
		iconYCoord = AutonomousMCGUI.map(yAccel, 1*g, -g, 0, 2 * background.getRadius());
		//Flipped because for computer coordinates positive y is down.
		
		//System.out.println(iconXCoord + " , " + iconYCoord);
		if(inRange(xAccel, yAccel)) { 
			icon.setLayoutX(iconXCoord);
			icon.setLayoutY(iconYCoord);
			icon.getStyleClass().removeAll("imuOutRange");
			icon.getStyleClass().add("imuInRange");
		}
		else { //Acceleration out of range
			icon.setLayoutX(background.getRadius());
			icon.setLayoutY(background.getRadius());
			icon.getStyleClass().removeAll("imuInRange");
			icon.getStyleClass().add("imuOutRange");
		}
	}

	private boolean inRange(double xAccel, double yAccel) {
		return( Math.pow(xAccel, 2) + Math.pow(yAccel, 2) <= Math.pow(1*g , 2) );
	}

}