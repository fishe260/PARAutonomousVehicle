package v2_0;

import java.io.InputStream;
import com.fazecast.jSerialComm.SerialPort;

public class AutonomousMCSerial implements Runnable{

	final int commandBytes = 1;
	
	boolean RFTActive;
	boolean FPVActive;
	volatile static boolean exitThread;
	
	//Serial communication vars
	static SerialPort telemetryPort;
	String kartPacketString;
	String[] kartPacketData;
	byte[] kartCommandBytes = new byte[commandBytes];
	InputStream telemetryStream;
	
	AutonomousMCFiles fileWriter;
	
	//Testing vars
	boolean flip1;
	boolean flip2;
	int xAccel;
	int yAccel;
	
	public AutonomousMCSerial(boolean radioConnected, boolean fpvConnected){
		RFTActive = radioConnected;
		FPVActive = fpvConnected;
		exitThread = false;
		
		fileWriter = new AutonomousMCFiles();

		SerialPort[] ports = SerialPort.getCommPorts();
		
		//Testing vars
		flip1 = false;
		flip2 = true;
		xAccel = 127;
		yAccel = 127;
		
		for(SerialPort i : ports)
			System.out.println(i.getDescriptivePortName() + " , " + i.getSystemPortName());
		
		if(radioConnected) {
			int i = 0;
			while(i < ports.length) {
				if(ports[i].getSystemPortName().equals("COM3") || ports[i].getSystemPortName().equals("COM4"))
					telemetryPort = ports[i];
				i++;
			}
			
			if(telemetryPort != null) {
				telemetryPort.openPort();
				telemetryPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_BLOCKING, 100, 0); //Only reads a certain amount of bytes at a time
				telemetryPort.setBaudRate(115200);
				telemetryStream = telemetryPort.getInputStream();
			}
			else {
				RFTActive = false;
				System.out.println("No Radio Connected");
			}
		}		
	}

	@Override
	public void run() {
		try {
			while(!exitThread) { //Run this thread until program closes

				if(RFTActive) {
					kartPacketString = "";
					while(!kartPacketString.contains("\n") && !exitThread) { //Read RFT_MC until newline char
						kartPacketString += (char)telemetryStream.read();
					}
					System.out.println(kartPacketString);	
					kartPacketData = kartPacketString.split(",");
					
					if(kartPacketData[0].charAt(0) >= 48 && kartPacketData[0].charAt(0) <= 57) {
						for(int i = 0 ; i < 12 ; i++) {	
							if(!exitThread)
								AutonomousMCGUI.kartPacket[i] = Double.parseDouble(kartPacketData[i]);
						}
					}
					
					kartCommandBytes[0] = (byte)(AutonomousMCGUI.stopBtnToggle ? 1 : 0);
					
					if(!exitThread) telemetryPort.writeBytes(kartCommandBytes, kartCommandBytes.length);
					
					if(AutonomousMCGUI.recBtnToggle && !exitThread) fileWriter.write(kartPacketString);	
				}
				
				if(FPVActive) {
					
				}
				
				if(!RFTActive && !FPVActive) {
					//For testing without USB
					
					if(xAccel <= 0)
						flip1 = true;
					else if(xAccel >= 255)
						flip1 = false;
					
					if(flip1)
						xAccel++;
					else if(!flip1)
						xAccel--;
						
					if(yAccel <= 0)
						flip2 = true;
					else if(yAccel >= 255)
						flip2 = false;
					
					if(flip2)
						yAccel+=2;
					else if(!flip2)
						yAccel-=2;
						
					Thread.sleep(10);
										
					AutonomousMCGUI.kartPacket[0] = xAccel; //Throttle (Dial)
					
					AutonomousMCGUI.kartPacket[2] = xAccel; //Desired steer (Steer)
					AutonomousMCGUI.kartPacket[3] = yAccel;	//Actual steer
					
					AutonomousMCGUI.kartPacket[6] = xAccel; //xAccel (IMU)
					AutonomousMCGUI.kartPacket[7] = 127;
					//AutonomousMCGUI.kartPacket[7] = yAccel; //yAccel (IMU)
					 
					AutonomousMCGUI.kartPacket[11] = xAccel; //bitflag
					
				}
				
			}
		} catch (Exception e) {e.printStackTrace();}
	}

	public static void closeAllPorts() {
		if(telemetryPort != null) {
			telemetryPort.removeDataListener();
			telemetryPort.closePort();
		}
	}

	public static void stopThreads() {
		exitThread = true;
	}

}