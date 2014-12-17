//package matrix_on_cloud_processing;
import java.io.*;
import java.util.*;

public class SchedulingLatency {

	public static void main(String[] args) throws IOException {
		// TODO Auto-generated method stub
		if (args.length != 3) {
			System.out.println("Usage: java SchedulingLatency workloadPath numNode numTask");
			System.exit(1);
		}
		
		int numNode = Integer.parseInt(args[1]);
		HashMap<String, TaskTime> hm = new HashMap<String, TaskTime>();
		
		BufferedReader br = null;
		
		for (int i = 0; i < numNode; i++) {
			br = new BufferedReader(new FileReader(args[0] + 
					"/task." + args[1] + "." + args[2] + "." + i));
			String str = br.readLine();
			while (str != null) {
				String[] strLine = str.split("\t");
				TaskTime tm = hm.get(strLine[0]);
				if (tm == null)
					tm = new TaskTime();
				if (strLine[1].equals("WaitQueueTime"))
					tm.wqTime = Long.parseLong(strLine[2]);
				if (strLine[1].equals("ReadyQueuedTime"))
					tm.rqTime = Long.parseLong(strLine[2]);
				if (strLine[1].equals("PushQueuedTime"))
					tm.pqTime = Long.parseLong(strLine[2]);
				if (strLine[1].equals("StartTime"))
					tm.startTime = Long.parseLong(strLine[2]);
				if (strLine[1].equals("FinTime"))
					tm.finTime = Long.parseLong(strLine[2]);
				
				hm.put(strLine[0], tm);
				
				str = br.readLine();
			}
			br.close();
		}
		
		Iterator<Map.Entry<String, TaskTime>> it = hm.entrySet().iterator();
		long sum = 0L;
		int size = hm.size();
		
		while (it.hasNext()) {
			Map.Entry<String, TaskTime> pair = it.next();
			TaskTime tm = pair.getValue();
			sum += tm.rqTime - tm.wqTime;
		}
		
		double aveScheLatency = (double)sum / (double)size;
		System.out.println(aveScheLatency);
	}
}

class TaskTime {
	long wqTime;
	long rqTime;
	long pqTime;
	long startTime;
	long finTime;
}
