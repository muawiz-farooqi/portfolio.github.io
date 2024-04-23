def decode(message_file):
	# Read the file and extract the lines
	with open(message_file, "r") as f:
		lines = f.readlines()
		lines = [line.split() for line in lines]

	# Create a dictionary mapping to store the number-word pairs
	map = {}
	for row in lines:
		map[int(row[0])] = row[1]

	# Find the decoded word using the pyramid decoding algorithm
	col_length = 1
	curr_line = []
	decoded_message = []
	for key in sorted(map.keys()):
		curr_line.append(map[key])
		if col_length == len(curr_line):
			decoded_message.append(map[key])
			curr_line = []
			col_length += 1
	
	# Join the decoded words into the final string
	decoded_message = " ".join(decoded_message)
	
	return decoded_message


def main():
	print(decode("message.txt"))

if __name__ == "__main__":
	main()