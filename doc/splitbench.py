import re

def splitFile(benchFile):
	freq = 0
	files = {}

	with open(benchFile, "r") as f:
		for line in f:
			match = re.match(r'^F_CPU_ACTUAL: (\d+)$', line)
			if match:
				freq = int(match[1])//1000000
				print('freq', freq)
				continue

			try:
				columns = [ l.strip() for l in line.split(',') ]
				print("columns", columns)
				id = int(columns[0])

				if freq not in files:
					files[freq] = open(f'plot-{freq}.csv', 'w')
				print(line, end='', file=files[freq])
			except Exception as e:
				print("exception", e)

splitFile("benchmark-max.txt")
