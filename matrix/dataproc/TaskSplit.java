//package matrix_on_cloud_processing;
import java.io.*;
import java.util.*;

public class TaskSplit {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		int numTask = Integer.parseInt(args[0]);
		int numNode = Integer.parseInt(args[1]);
		int ave = numTask / numNode;
		Random ran = new Random();
		int toNode = -1;
		
		ArrayList<String>[] al = new ArrayList[numNode];
		for (int i = 0; i < numNode; i++)
			al[i] = new ArrayList<String>();
		for (int i = 0; i < numTask; i++) {
			toNode = ran.nextInt(numNode);
			while (al[toNode].size() >= ave)
				toNode = ran.nextInt(numNode);
			al[toNode].add("0" + i + " kwang /home/ hostname");
		}
		for (int i = 0; i < numNode; i++) {
			BufferedWriter bw = new BufferedWriter(new FileWriter("./workload." + i));
			for (int j = 0; j < al[i].size(); j++)
				bw.write(al[i].get(j) + "\n");
			bw.flush();
			bw.close();
		}
	}

}
