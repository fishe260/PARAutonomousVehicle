package v2_2;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Line;
import javafx.scene.shape.Rectangle;

public class AutonomousMCBarMeter extends HBox {
	
	int lowerBound;
	int upperBound;
	boolean centered;
	
	Rectangle indicator;
	
	final int barWidth = 300;
	final int barHeight = 20;
	
	public AutonomousMCBarMeter(String label, int lower, int upper, boolean centerSet) {
		super(5);
		this.setAlignment(Pos.BOTTOM_CENTER);
		centered = centerSet;
		lowerBound = lower;
		upperBound = upper;
				
		Label barName = new Label(String.format("%3s", label));
		barName.getStyleClass().add("barTitle");
		Label barMin = new Label(String.format("%4s", lower));
		VBox meter = new VBox(5);
		meter.setAlignment(Pos.CENTER);
		StackPane rectangleAndTick = new StackPane();
		Rectangle barOutline = new Rectangle(barWidth, barHeight, Color.WHITE);
		barOutline.setStroke(Color.BLACK);
		barOutline.setStrokeWidth(2);
		rectangleAndTick.getChildren().add(barOutline);
		indicator = new Rectangle(10, barHeight - 2, Color.TRANSPARENT);
		indicator.setStroke(Color.BLACK);
		indicator.setLayoutX(2);
		indicator.setLayoutY(2);
		indicator.setManaged(false);
		rectangleAndTick.getChildren().add(indicator);
		if(centerSet) {
			//Center tick mark
			Line tickMark = new Line(0, 0, 0, barHeight - 3);
			tickMark.setStrokeWidth(3);
			rectangleAndTick.getChildren().add(tickMark);
			//Center Label
			Label barCenter = new Label(String.format("%.1f", 0.5*(lower + upper)));
			meter.getChildren().add(barCenter);
		}
		meter.getChildren().add(rectangleAndTick);

		Label barMax = new Label(String.format("%-4s", upper));
		
		this.getChildren().add(barName);
		this.getChildren().add(barMin);
		this.getChildren().add(meter);
		this.getChildren().add(barMax);
	}
	
	public void setValue(double value){
		
		if(value > upperBound) { //Out of bounds
			indicator.setWidth(  barWidth );
			indicator.setFill(Color.CYAN);
		}
		else if(value < lowerBound) {
			indicator.setWidth( 2 );
			indicator.setFill(Color.SADDLEBROWN);
		}
		else {
			//Map value to bar size
			indicator.setWidth(  barWidth * (value - lowerBound)/(upperBound - lowerBound)   );
					
			//Map value to bar color
			int rVal = 0;
			int gVal = 0;
			int bVal = 0;
			if(value - lowerBound <= 0.5 * (upperBound - lowerBound)) { //first half
				rVal = (int)(255 - 255*(value - lowerBound)/(0.5 * (upperBound - lowerBound)));
				gVal = (int)(255 * (value - lowerBound)/(0.5 * (upperBound - lowerBound)));
			}
			else {
				bVal = (int)(255 * (value - (0.5 * (upperBound + lowerBound))) / (0.5 * (upperBound - lowerBound)));
				gVal = (int)(255 - 255 * (value - (0.5 * (upperBound + lowerBound))) / (0.5 * (upperBound - lowerBound)));
			}
			indicator.setFill(Color.rgb(rVal, gVal, bVal));
		}
		
	}

	
}
