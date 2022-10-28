import random
import string

letters = string.ascii_lowercase

print("CREATE TABLE apa (name varchar(20), weight int);");

for i in range(20000):
    name=''.join(random.choice(letters) for i in range(8))
    print("INSERT INTO apa VALUES (\""+name+"\", "+str(i)+");")
