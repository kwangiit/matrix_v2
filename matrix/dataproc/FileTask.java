//package matrix_on_cloud_processing;

import java.io.*;
import java.util.*;

public class FileTask {

	public static void main(String[] args) throws IOException{
		// TODO Auto-generated method stub
		Random ran = new Random();
		int numFile = Integer.parseInt(args[0]);
		int numNode = Integer.parseInt(args[1]);
		int numTask = Integer.parseInt(args[2]);
		int numMapTask = Integer.parseInt(args[3]);
		
		BufferedReader br = new BufferedReader(
				new FileReader("./yarn_task_" + args[1] + "_" + args[2]));
		ArrayList<String> al = new ArrayList<String>();
		String str = br.readLine();
		while (str != null) {
			al.add(str.substring(0));
			str = br.readLine();
		}
		
		BufferedWriter bw = new BufferedWriter(new FileWriter("./file_" + args[1] + "_" + args[2]));
		String fileStr = "file_";
		int toNode = -1;
		for (int i = 0; i < numFile; i++) {
			toNode = i % numNode;
			bw.write(fileStr + i + " " + toNode + "\n");
		}
		bw.flush();
		bw.close();
		
		bw = new BufferedWriter(new FileWriter("./task_" + args[1] + "_" + args[2]));
		boolean[] fileTaken = new boolean[numFile];
		for (int i = 0; i < numFile; i++)
			fileTaken[i] = false;
		
		for (int i = 0; i < numMapTask; i++) {
			toNode = ran.nextInt(numFile);
			while (fileTaken[toNode]) {
				toNode = ran.nextInt(numFile);
			}
			fileTaken[toNode] = true;
			bw.write(i + " " + al.get(i) + " " + fileStr + toNode + "\n");
		}
		int numReduceTask = numTask - numMapTask;
		for (int i = 0; i < numReduceTask; i++)
			bw.write((i + numMapTask) + " " + al.get(i + numMapTask) + "\n");
		bw.flush();
		bw.close();
		br.close();
	}

}
