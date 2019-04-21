package v2_2;

import java.util.ArrayList;

import javafx.animation.AnimationTimer;
import javafx.event.ActionEvent;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.Menu;
import javafx.scene.control.MenuBar;
import javafx.scene.control.MenuItem;
import javafx.scene.image.Image;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.Background;
import javafx.scene.layout.BackgroundFill;
import javafx.scene.layout.BackgroundImage;
import javafx.scene.layout.BackgroundPosition;
import javafx.scene.layout.BackgroundRepeat;
import javafx.scene.layout.BackgroundSize;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;

public class AutonomousMCGUI {
	
	static double kartPacket[];
	
	static boolean stopBtnToggle; //Stop button toggle state
	static boolean recBtnToggle; //Record button toggle state
	
	//Background image vars
	BackgroundImage backgroundImage;
	Background myBackground;
	
	//Animation timer vars
	long oldFrameTime;
	long oldRecFrameTime;
	short frameCount;
	
	//updateGUI() vars
	
	final static String[] modeText = {"0- Kart Diabled", "1- RC", 
			"2- Training", "3- Qualifing", "4- Race", "5- Waiting on Mode Confirmation"};
	
	static byte bitFlag;
	ArrayList<HBox> statusDisplay;
	static ArrayList<Circle> statusLights;
	static AutonomousMCMode modeDisplay;
	static VBox powerDisplay;
	static VBox steeringAndThrottleDisplay;
	static AutonomousMCIMU imuDisplay;
	
	public AutonomousMCGUI(AnchorPane anchorPane) {
		//Initialize global variables
		kartPacket = new double[12];
		
		//Background
		myBackground = new Background(new BackgroundFill(Color.BLACK, null, null));
		anchorPane.setBackground(myBackground);
		
		//Menu Bar
		Menu backgroundMenu = new Menu("Background");
		
		MenuItem solidWhiteBackground = new MenuItem("White");
		solidWhiteBackground.setOnAction(e -> {
			myBackground = new Background(new BackgroundFill(Color.WHITE , null, null));
			anchorPane.setBackground(myBackground);
		});
		
		MenuItem solidBlackBackground = new MenuItem("Black");
		solidBlackBackground.setOnAction(e -> {
			myBackground = new Background(new BackgroundFill(Color.BLACK, null, null));
			anchorPane.setBackground(myBackground);
		});
		
		MenuItem trippyBackground = new MenuItem("Trippy");
		trippyBackground.setOnAction(e -> {
			changeBackground("/resources/Trippy.gif", anchorPane);
		});
		
		MenuItem keyboardCatBackground = new MenuItem("Keyboard Cat");
		keyboardCatBackground.setOnAction(e -> {
			changeBackground("/resources/keyboardCat.gif", anchorPane);
		});
		
		MenuItem secretBackground = new MenuItem("???"); //Random
		secretBackground.setOnAction(e -> {
			changeBackground("/resources/Shaq.gif", anchorPane);
		});
		
		backgroundMenu.getItems().addAll(solidWhiteBackground, solidBlackBackground, trippyBackground,
				keyboardCatBackground, secretBackground);
		
		MenuBar menuBar = new MenuBar();
		menuBar.getMenus().add(backgroundMenu);
		menuBar.setMinWidth(anchorPane.getWidth());
		
		AnchorPane.setTopAnchor(menuBar, 0.0);
		anchorPane.getChildren().add(menuBar);
		
		//Mode Display
		modeDisplay = new AutonomousMCMode();
		AnchorPane.setLeftAnchor(modeDisplay, 10.0);
		AnchorPane.setBottomAnchor(modeDisplay, 500.0);
		anchorPane.getChildren().add(modeDisplay);
		
		//FPS Counter
		Label fpsLabel = new Label();
		fpsLabel.getStyleClass().add("fpsLabel");
		fpsLabel.setText("FPS: ");
		frameCount = 0;

		AnimationTimer frameRateMeter = new AnimationTimer() {
			@Override
			public void handle(long now) {
				updateGUI();
				
				if(frameCount == 0)
					oldFrameTime = now;
				
				frameCount++;
				
				if(now - oldFrameTime >= 1_000_000_000) {
					fpsLabel.setText(String.format("FPS: %d", frameCount));
					frameCount = 0;
				}	
			}	
		};
		frameRateMeter.start();
		
		AnchorPane.setRightAnchor(fpsLabel, 10.0);
		AnchorPane.setTopAnchor(fpsLabel, 10.0);
		anchorPane.getChildren().addAll(fpsLabel);
		
		//EStop Button
		Button estopBtn = new Button("STOP");
		Circle estopBtnShape = new Circle(75);
		
		estopBtn.setShape(estopBtnShape);
		estopBtn.setMinSize(75,75); //Prevents it from being squashed around text
		estopBtn.getStyleClass().add("eStopDefault");
		stopBtnToggle = false;
		
		estopBtn.setOnAction((ActionEvent e) -> {
			stopBtnToggle ^= true;
			if(stopBtnToggle) {
				estopBtn.setText("RESUME");
				estopBtn.getStyleClass().removeAll("eStopDefault");
				estopBtn.getStyleClass().add("eStopEngaged");
				modeDisplay.sendEStop();
			}
			else {
				estopBtn.setText("STOP");
				estopBtn.getStyleClass().removeAll("eStopEngaged");
				estopBtn.getStyleClass().add("eStopDefault");
				modeDisplay.revokeEStop();
			}
		});
		
		//Record Button
		Button recBtn = new Button();
		Rectangle recBtnShape = new Rectangle(25,25);
		recBtn.setShape(recBtnShape);
		recBtn.setMinSize(50, 50);
		recBtn.getStyleClass().add("recBtnDefault");
		recBtnToggle = false;
		
		AnimationTimer recordButtonBlink = new AnimationTimer() {
			@Override
			public void handle(long now) {
				if(now - oldRecFrameTime >= 500_000_000) {
					oldRecFrameTime = now;
					if(recBtn.getStyleClass().contains("recBtnDefault")) {
						recBtn.getStyleClass().removeAll("recBtnDefault");
						recBtn.getStyleClass().add("recBtnEngaged");
					}
					else {
						recBtn.getStyleClass().removeAll("recBtnEngaged");
						recBtn.getStyleClass().add("recBtnDefault");
					}
				}
			}
		};
		
		recBtn.setOnAction((ActionEvent e) ->{
			recBtnToggle ^= true;
			if(recBtnToggle) {
				recordButtonBlink.start();
			}
			else {
				recBtn.getStyleClass().removeAll("recBtnEngaged");
				recBtn.getStyleClass().add("recBtnDefault");
				recordButtonBlink.stop();
			}
		});
		
		VBox commandBtns = new VBox(25);
		commandBtns.setAlignment(Pos.CENTER);
		commandBtns.getStyleClass().add("btnsBackground");
		commandBtns.getChildren().add(recBtn);
		commandBtns.getChildren().add(estopBtn);
		commandBtns.setMinSize(80, 160);

		AnchorPane.setLeftAnchor(commandBtns, 10.0);
		AnchorPane.setBottomAnchor(commandBtns, 300.0);
		anchorPane.getChildren().add(commandBtns);
		
		//Status Indicators
		VBox statusPane = new VBox(10);
		statusPane.setAlignment(Pos.CENTER);
		statusPane.getStyleClass().add("statusBackground");
		
		String[] statusLabels = {"CPU", "STEERING", "THROTTLE", "COMM RADIO", "LOCATION", "POWER", "IMU", "TEL RADIO"}; 
		
		statusDisplay = new ArrayList<HBox>();
		statusLights = new ArrayList<Circle>();
		
		for(int i = 0 ; i < statusLabels.length ; i++) {
			statusLights.add(new Circle(8, Color.CRIMSON));
			statusLights.get(i).getStyleClass().add("statusLightOff");
			statusDisplay.add(new HBox(15));
			statusDisplay.get(i).setAlignment(Pos.CENTER);
			statusDisplay.get(i).getChildren().add(new Label(String.format("%10s",statusLabels[i])));
			statusDisplay.get(i).getChildren().add(statusLights.get(i));
			statusPane.getChildren().add(statusDisplay.get(i));
		}
		
		AnchorPane.setBottomAnchor(statusPane, 10.0);
		AnchorPane.setLeftAnchor(statusPane, 10.0);
		statusPane.setPadding(new Insets(5,5,5,5));
		anchorPane.getChildren().add(statusPane);
		
		//Power Display
		powerDisplay = new VBox(10);
		powerDisplay.setPadding(new Insets(5,5,5,5));
		powerDisplay.getStyleClass().add("barMeterBackground");
		powerDisplay.getChildren().add(new AutonomousMCBarMeter("V", 40, 56, true));
		powerDisplay.getChildren().add(new AutonomousMCBarMeter("I", 0, 300, false));
		powerDisplay.getChildren().add(new AutonomousMCBarMeter("P", 0, 10, false));
		AnchorPane.setTopAnchor(powerDisplay, 10.0);
		AnchorPane.setLeftAnchor(powerDisplay, 350.0);
		AnchorPane.setRightAnchor(powerDisplay, 350.0);
		anchorPane.getChildren().add(powerDisplay);
		
		//Steering and throttle display
		steeringAndThrottleDisplay = new VBox(10);
		steeringAndThrottleDisplay.setAlignment(Pos.CENTER);
		steeringAndThrottleDisplay.getChildren().add(new AutonomousMCSteering());
		steeringAndThrottleDisplay.getChildren().add(new AutonomousMCThrottle());
		AnchorPane.setBottomAnchor(steeringAndThrottleDisplay, 0.0);
		AnchorPane.setLeftAnchor(steeringAndThrottleDisplay, 350.0);
		AnchorPane.setRightAnchor(steeringAndThrottleDisplay, 350.0);
		anchorPane.getChildren().add(steeringAndThrottleDisplay);
		
		//Processing Speed display
		
		//IMU
		imuDisplay = new AutonomousMCIMU();
		AnchorPane.setBottomAnchor(imuDisplay, 10.0);
		AnchorPane.setRightAnchor(imuDisplay, 10.0);
		anchorPane.getChildren().add(imuDisplay);
	}
	
	public static void updateGUI() {	
		//Mode Update
		modeDisplay.setActualValue(kartPacket[8]);
		
		//System Status Update
		bitFlag = (byte) kartPacket[11];
		for(int i = 0 ; i < 8 ; i++) {
			if(((bitFlag >> i) & 1) == 0) {
				statusLights.get(i).getStyleClass().removeAll("statusLightOn");
				statusLights.get(i).getStyleClass().add("statusLightOff");
			}
			else {
				statusLights.get(i).getStyleClass().removeAll("statusLightOff");
				statusLights.get(i).getStyleClass().add("statusLightOn");
			}
		}
		
		//Power Update
		((AutonomousMCBarMeter) powerDisplay.getChildren().get(0)).setValue(kartPacket[4] / 100.0); //Voltage
		((AutonomousMCBarMeter) powerDisplay.getChildren().get(1)).setValue(kartPacket[5] / 100.0); //Current
		((AutonomousMCBarMeter) powerDisplay.getChildren().get(2)).setValue((kartPacket[4] * kartPacket[5] / 1E7)); //Power

		//Steering
		((AutonomousMCSteering) steeringAndThrottleDisplay.getChildren().get(0)).setValue(kartPacket[2], kartPacket[3]);
		
		//Throttle
		((AutonomousMCThrottle) steeringAndThrottleDisplay.getChildren().get(1)).setValue(kartPacket[0], kartPacket[1]);
		
		//Processing Speed Update
		
		
		//IMU Update
		imuDisplay.setValue(kartPacket[6], kartPacket[7]);
	}

	public static double map(double val, double iLo, double iHi, double fLo, double fHi) {
		return (fLo + (fHi - fLo) * ((val - iLo) / (iHi - iLo)));
	}
	
	public void changeBackground(String imageURL, AnchorPane anchorPane) {
		backgroundImage = new BackgroundImage(new Image(imageURL),
				BackgroundRepeat.NO_REPEAT, //X repeat
				BackgroundRepeat.NO_REPEAT, //Y repeat
				BackgroundPosition.CENTER, //Position
				new BackgroundSize(anchorPane.getWidth(), anchorPane.getHeight(), false, false, false, true)); //Size
		myBackground = new Background(backgroundImage);
		anchorPane.setBackground(myBackground);
	}
}