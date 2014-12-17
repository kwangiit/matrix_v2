import java.io.*;
import java.util.*;

public class YarnLogProc {
	public static void main(String[] args) throws IOException {
		if (args.length != 4) {
			System.out.println("Usage:java YarnLogProc numNode fileSize workloadType numMapTask");
			System.exit(1);
		}
		/* args[0]: number of nodes
		 * args[1]: file size
		 * args[2]: workload type
		 * args[3]: number of map tasks
		*/
		int fileSize = Integer.parseInt(args[1]);
		int numMapTask = Integer.parseInt(args[3]);
		int numAllTask = numMapTask + 1;
		long inputSizePerTask = (long)fileSize * 1024L * 1024L / numMapTask; 
		
		String filePath = "/home/kwang/Documents/work_kwang/"
				+ "MATRIX_on_Cloud/YARN_Results/logcenter/"
				+ args[0] + "_instances/";
		
		HashMap<String, Task> hmTask = new HashMap<String, Task>();
		BufferedReader br = new BufferedReader(new FileReader(filePath + args[2] + ".map"));
		System.out.println("The file is:" + filePath + args[2] + ".map");
		String str = br.readLine();
		while (str != null) {
			//System.out.println("The line is:" + str);
			String[] strLine = str.split(" |\t\t\t|\t");
			if (strLine.length < 25) {
				str = br.readLine();
				continue;
			}
			else {
//				for (int i = 0; i < strLine.length; i++)
//					System.out.println(strLine[i]);
				//System.out.println(strLine.length);
				Task task = new Task();
				task.taskId = strLine[0].split("_")[4];
				System.out.println(task.taskId);
				task.taskType = "map";
				task.inputSize = inputSizePerTask;
				//System.out.println(strLine[14]);
				String taskLengthStr = strLine[14].substring(0, strLine[14].length() - 3);
				task.taskLength = Long.parseLong(taskLengthStr) * 1000000;
				hmTask.put(task.taskId, task);
			}
			str = br.readLine();
		}
		br.close();
		
		File folder = new File(filePath + "selflog");
		File[] listOfFiles = folder.listFiles();
		System.out.println(listOfFiles.length);
		for (int i = 0; i < listOfFiles.length; i++) {
			String fileName = listOfFiles[i].getName();
			if (fileName.compareTo("MapTaskID.txt") != 0) {
				//System.out.println(fileName);
				String[] strLineTmp = fileName.split("_");
					if (strLineTmp[2].equals("0002")) {
					String taskId = fileName.split("_")[4];
					//System.out.println(listOfFiles[i].getAbsolutePath());
					br = new BufferedReader(new FileReader(listOfFiles[i].getAbsolutePath()));
					str = br.readLine();
					long taskOutputSize = Long.parseLong(str.split("\\s+")[3]);
					Task task = hmTask.get(taskId);
					task.outputSize = taskOutputSize;
					System.out.println("The length is:" + taskOutputSize);
					hmTask.put(taskId, task);
					br.close();
				}
			}
		}
		
		BufferedWriter bw = new BufferedWriter(new FileWriter("/home/kwang/Documents/work_kwang/"
				+ "MATRIX_on_Cloud/Workload/yarn_task_" + args[0] + "_" + Integer.toString(numAllTask)));
		Iterator it = hmTask.entrySet().iterator();
		ArrayList<Task> al = new ArrayList<Task>();
		long length = 0L;
		while (it.hasNext()) {
			Map.Entry<String, Task> pairs = (Map.Entry)it.next();
			al.add(pairs.getValue());
			length += pairs.getValue().taskLength;
			//bw.write(task.taskType + " " + task.inputSize + " " + task.outputSize + " " + task.taskLength + "\n");			
		}
		long ave = length / (long)(al.size());
		length = 0L;
		long count = 0L;
		for (int i = 0; i < al.size(); i++) {
			if (al.get(i).taskLength < ave) {
				length += al.get(i).taskLength;
				count++;
			}
		}
		long aveLess = length / count;
		for (int i = 0; i < al.size(); i++) {
			if (al.get(i).taskLength > ave)
				al.get(i).taskLength = aveLess;
		}
		for (int i = 0; i < al.size(); i++) {
			Task task = al.get(i);
			bw.write(task.taskType + " " + task.inputSize + " " + task.outputSize + " " + task.taskLength + "\n");
		}
			
		bw.flush();
		bw.close();
		long outputSizeSum = 0L;
		for (int i = 0; i < al.size(); i++)
			outputSizeSum += al.get(i).outputSize;
		System.out.println("The output size is:" + outputSizeSum);
	}
}

class Task {
	String taskId;
	String taskType;
	long inputSize;
	long outputSize;
	long taskLength;
}
