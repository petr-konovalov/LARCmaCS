with open("./sent_commands.log") as f:
	lines = f.readlines()


for line in lines:
	splitted = line.split(" at ")
	timestamp = splitted[1].split(" ")[0]
	