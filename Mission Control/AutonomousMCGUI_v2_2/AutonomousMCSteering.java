package v2_2;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Group;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.scene.shape.Arc;
import javafx.scene.shape.ArcType;

public class AutonomousMCSteering extends StackPane {
	
	final double steeringLimit = 45;
	Arc actualArc;
	double actualArcLength; 
	Arc desiredArc;
	double desiredArcLength;
	Group actualGroup, desiredGroup;
	
	public AutonomousMCSteering() {
		super();
		
		this.setAlignment(Pos.BOTTOM_CENTER);
		this.setPadding(new Insets(5,5,5,5));
		this.getStyleClass().add("steeringBackground");
				
		Arc outerSemicircle = new Arc();
		outerSemicircle.setType(ArcType.OPEN);
		outerSemicircle.setStroke(Color.BLACK);
		outerSemicircle.setStrokeWidth(2);
		outerSemicircle.setRadiusX(50);
		outerSemicircle.setRadiusY(50);
		outerSemicircle.setStartAngle(0f);
		outerSemicircle.setLength(180f);
		outerSemicircle.setFill(Color.TRANSPARENT);	
			
		actualArc = new Arc();
		actualArc.setType(ArcType.OPEN);
		actualArc.setStroke(Color.ROYALBLUE);
		actualArc.setStrokeWidth(6);
		actualArc.setFill(Color.TRANSPARENT);
		actualArc.setRadiusX(50);
		actualArc.setRadiusY(50);
		actualArc.setStartAngle(90f);
		actualArc.setLength(0f);
		
		actualGroup = new Group(outerSemicircle, actualArc);
		this.getChildren().add(actualGroup);
		actualGroup.setLayoutY(55);
		actualGroup.setManaged(false);
		
		Arc innerSemicircle = new Arc();
		innerSemicircle.setType(ArcType.OPEN);
		innerSemicircle.setStroke(Color.BLACK);
		innerSemicircle.setStrokeWidth(2);
		innerSemicircle.setRadiusX(25);
		innerSemicircle.setRadiusY(25);
		innerSemicircle.setStartAngle(0f);
		innerSemicircle.setLength(180f);
		innerSemicircle.setFill(Color.TRANSPARENT);
		
		desiredArc = new Arc();
		desiredArc.setType(ArcType.OPEN);
		desiredArc.setStroke(Color.GREEN);
		desiredArc.setStrokeWidth(6);
		desiredArc.setFill(Color.TRANSPARENT);
		desiredArc.setRadiusX(25);
		desiredArc.setRadiusY(25);
		desiredArc.setStartAngle(90f);
		desiredArc.setLength(0f);
		
		desiredGroup = new Group(innerSemicircle, desiredArc);
		this.getChildren().add(desiredGroup);
		desiredGroup.setLayoutY(55);
		desiredGroup.setManaged(false);
		
		Image SteerImage = new Image(getClass().getResourceAsStream("/resources/SteeringWheel.png"));
		ImageView steerIcon = new ImageView(SteerImage);
		steerIcon.setFitHeight(20);
		steerIcon.setFitWidth(20);
		this.getChildren().add(steerIcon);	
		this.setMinHeight(60);
	}
	
	public void setValue(double desired, double actual) {
		actualArcLength = AutonomousMCGUI.map(actual, 0, 255, 90, -90);
		desiredArcLength = AutonomousMCGUI.map(desired, 0, 255, 90, -90);
		
		desiredGroup.setLayoutX(this.getWidth() / 2);
		actualGroup.setLayoutX(this.getWidth() / 2);

		actualArc.setLength(actualArcLength);
		desiredArc.setLength(desiredArcLength);
	}

}