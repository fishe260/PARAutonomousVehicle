package v2_1;

import java.io.*;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class AutonomousMCFiles {

	static FileOutputStream fileOut;
	
	Date date;
	DateFormat lineDateFormat;
	
	public AutonomousMCFiles(){
		date = new Date();
		DateFormat fileDateFormat = new SimpleDateFormat("yyyy-MM-dd");
		String fileName = fileDateFormat.format(date) + "_KartData.csv";
		new File("c://AutonomousKart").mkdirs();
		File csvFile = new File("c://AutonomousKart//" + fileName);
		
		int version = 1;
		try {
			while(!csvFile.createNewFile() && version < 1000) {
				//Subtract end chars
				if(version == 1)
					fileName = fileName.substring(0, fileName.length() - 4);
				else if (version <= 10)
					fileName = fileName.substring(0, fileName.length() - 7);
				else if (version <= 100)
					fileName = fileName.substring(0, fileName.length() - 8);
				else
					fileName = fileName.substring(0, fileName.length() - 9); 
				/* Supports naming up to 999 files in the same day! WOW!
				Any more will overwrite the last file, 999
				It would literally just take two lines to add in support for up to 9999
				But I don't forsee anyone closing and opening this program 1000 times in a day. 
				*/
				
				//Add chars
				fileName += "(" + Integer.toString(version) + ").csv";
				csvFile = new File("c://AutonomousKart//" + fileName);
				version++;
			}
		} catch (IOException e) {e.printStackTrace();}
		
		lineDateFormat = new SimpleDateFormat("HH:mm:ss.SSS");

		try {
			fileOut = new FileOutputStream(csvFile);
		} catch (FileNotFoundException e) {e.printStackTrace();}
		
		String headerLine = "Time, Throttle, Speed, Desired Steering, Actual Steering, Voltage, Current, xAccel, yAccel, Mode, RFT PPS, CPU IPS, Bitflag\n";
		try {
			fileOut.write(headerLine.getBytes());
		} catch (IOException e) { e.printStackTrace(); }
		
	}
	
	public void write(String line){
		//Append the time to this string
		date.setTime(System.currentTimeMillis());
		line = lineDateFormat.format(date) + "," + line;
		try {
			fileOut.write(line.getBytes());
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	public static void closeFiles(){
		if(fileOut != null) {
			try {
				fileOut.flush();
				fileOut.close();
			}
			catch(IOException e) {e.printStackTrace();}
		}
	}
	
}