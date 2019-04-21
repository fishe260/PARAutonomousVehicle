package v2_0;

import java.util.ArrayList;

import javafx.animation.AnimationTimer;
import javafx.event.ActionEvent;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.shape.Circle;
import javafx.scene.shape.Rectangle;

public class AutonomousMCGUI {
	
	static double kartPacket[];
	
	static boolean stopBtnToggle; //Stop button toggle state
	static boolean recBtnToggle; //Record button toggle state
	
	//Animation timer vars
	long oldFrameTime;
	long oldRecFrameTime;
	short frameCount;
	
	//updateGUI() vars
	static Label modeDisplay;
	final static String[] modeText = {"0- Kart Diabled", "1- RC", 
			"2- Training", "3- Qualifing", "4- Race", "5- Unused", 
			"6- Unused", "7- Waiting on Mode Confirmation"};
	
	static byte bitFlag;
	ArrayList<HBox> statusDisplay;
	static ArrayList<Circle> statusLights;
	
	static ArrayList<AutonomousMCDial> dialPanes;
	final static double[] dialsMin = {0, 0, 0, 38, 0, 0, 0, 0};
	final static double[] dialsMax = {100, 100, 40, 56, 300, 10, 400, 100};
	static double[] dialVals = new double[dialsMax.length];
	static AutonomousMCIMU imuDisplay;
	static AutonomousMCSteering steerDisplay;
	
	public AutonomousMCGUI(AnchorPane anchorPane) {
		//Initialize global variables
		kartPacket = new double[12];
				
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
		
		AnchorPane.setLeftAnchor(fpsLabel, 10.0);
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
			}
			else {
				estopBtn.setText("STOP");
				estopBtn.getStyleClass().removeAll("eStopEngaged");
				estopBtn.getStyleClass().add("eStopDefault");
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
		
		//Mode Display & Status Indicators
		VBox modeAndStatusPane = new VBox(10);
		modeAndStatusPane.setAlignment(Pos.CENTER);
		modeAndStatusPane.getStyleClass().add("modeAndStatusBackground");
		
		Label modeLabel = new Label("Current Mode: ");
		modeLabel.getStyleClass().add("modeDisplay");
		modeAndStatusPane.getChildren().add(modeLabel);
		
		modeDisplay = new Label();
		modeDisplay.getStyleClass().add("modeDisplay");
		modeAndStatusPane.getChildren().add(modeDisplay);	
		
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
			modeAndStatusPane.getChildren().add(statusDisplay.get(i));
		}
		
		AnchorPane.setBottomAnchor(modeAndStatusPane, 10.0);
		AnchorPane.setLeftAnchor(modeAndStatusPane, 10.0);
		modeAndStatusPane.setPadding(new Insets(5,5,5,5));
		anchorPane.getChildren().add(modeAndStatusPane);
		
		//Dials
		String[] dialLabels = {"THROTTLE", "BRAKE", "SPEED", "VOLTAGE", "CURRENT", "POWER", "RFT", "CPU"};
		String[] dialUnits = {"%", "%", "MPH", "V", "A", "kW", "PPS", "IPS"};
		HBox dialDisplay = new HBox(25);
		dialDisplay.getStyleClass().add("dialsBackground");
		dialDisplay.setAlignment(Pos.CENTER);
		dialDisplay.setMinWidth(1000);
		
		dialPanes = new ArrayList<AutonomousMCDial>();
		for(int i = 0; i < dialLabels.length ; i++) {
			dialPanes.add(new AutonomousMCDial(dialLabels[i], dialUnits[i]));
			dialDisplay.getChildren().add(dialPanes.get(i));
		}
		
		AnchorPane.setTopAnchor(dialDisplay, 10.0);
		AnchorPane.setLeftAnchor(dialDisplay, 175.0);
		AnchorPane.setRightAnchor(dialDisplay, 175.0);
		anchorPane.getChildren().add(dialDisplay);
		
		dialDisplay.setMaxSize(dialDisplay.getWidth(), dialDisplay.getHeight()); //Just testing this out
		
		//Steering Meter & IMU
		steerDisplay = new AutonomousMCSteering();
		imuDisplay = new AutonomousMCIMU();
		VBox steerAndIMU = new VBox(10);
		steerAndIMU.getChildren().add(steerDisplay);
		steerAndIMU.getChildren().add(imuDisplay);
	
		AnchorPane.setBottomAnchor(steerAndIMU, 10.0);
		AnchorPane.setRightAnchor(steerAndIMU, 10.0);
		anchorPane.getChildren().add(steerAndIMU);
	}
	
	public static void updateGUI() {	
		//Mode Update
		modeDisplay.setText(modeText[(int)kartPacket[8]]);
		
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
		
		//Dials Update
		if(kartPacket[0] >= 127) {
			dialVals[0] = map(kartPacket[0], 127, 255, 0, 100); //Throttle
			dialVals[1] = 0; //Brake
		}
		else {
			dialVals[0] = 0;
			dialVals[1] = map(kartPacket[0], 127, 0, 0, 100);
		}
		dialVals[2] = kartPacket[1];//Speed
		dialVals[3] = kartPacket[4] / 100;//Voltage
		dialVals[4] = kartPacket[5] / 100;//Current
		dialVals[5] = (dialVals[3] * dialVals[4] / 1000.0); //Power
		dialVals[6] = kartPacket[9]; //RFT
		dialVals[7] = kartPacket[10]; //CPU
		for(int i = 0 ; i < dialPanes.size(); i++) {
			if(i == 3 || i == 4 || i == 5) //Display as decimals
				dialPanes.get(i).setValue(dialsMin[i], dialsMax[i], dialVals[i], false);
			else //Display as int
				dialPanes.get(i).setValue(dialsMin[i], dialsMax[i], dialVals[i], true);
		}
		
		//Steering Meter Update
		steerDisplay.setValue(kartPacket[2], kartPacket[3]);
		
		//IMU Update
		imuDisplay.setValue(kartPacket[6], kartPacket[7]);
	}

	public static double map(double val, double iLo, double iHi, double fLo, double fHi) {
		return (fLo + (fHi - fLo) * ((val - iLo) / (iHi - iLo)));
	}
}