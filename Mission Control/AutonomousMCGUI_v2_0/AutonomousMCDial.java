package v2_0;

import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.control.Label;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Arc;
import javafx.scene.shape.ArcType;
import javafx.scene.shape.Circle;

public class AutonomousMCDial extends StackPane {

	double newLength; //New length of arc
	Arc dialArc;
	Label dialNumber;
		
	public AutonomousMCDial(String label, String unit) {
		super();
		Circle dialCircle = new Circle(40, Color.LIGHTSLATEGRAY);
		dialCircle.setStroke(Color.BLACK);
		dialCircle.setStrokeWidth(2);
		
		dialArc = new Arc();
		dialArc.setType(ArcType.OPEN);
		dialArc.setStroke(Color.LIMEGREEN);
		dialArc.setStrokeWidth(4);
		dialArc.setFill(Color.TRANSPARENT);
		dialArc.setRadiusX(42);
		dialArc.setRadiusY(42);
		dialArc.setStartAngle(90f);
		dialArc.setLength(0f);
		
		VBox dialContents = new VBox(8);
		dialContents.setAlignment(Pos.CENTER);
		Label dialLabel = new Label(label);
		dialNumber = new Label();
		Label dialUnit = new Label(unit);
		dialLabel.getStyleClass().add("dialText");
		dialNumber.getStyleClass().add("dialNumber");
		dialUnit.getStyleClass().add("dialText");
		dialContents.getChildren().addAll(dialLabel, dialNumber, dialUnit);
		
		Group dialGroup = new Group(dialCircle, dialArc);
		
		this.getChildren().add(dialGroup);
		this.getChildren().add(dialContents);
		this.setMinSize(90, 90);
		
		dialGroup.setManaged(false);
		dialGroup.setLayoutX(45);
		dialGroup.setLayoutY(45);
	}
	
	public void setValue(double min, double max, double val, boolean integer) {
		//Change text
		if(integer)
			dialNumber.setText(String.format("%d", (int)val));
		else 
			dialNumber.setText(String.format("%.2f", val));
		
		//Change Arc
		newLength = AutonomousMCGUI.map(val, min, max, 0f, 360f);
		dialArc.setLength(newLength);
	}

}