import random
import string

def generate_random_string(length=10):
    return ''.join(random.choice(string.ascii_letters) for _ in range(length))

def generate_xml_template(random_string):
    template = '''
<publicUserId>
	<publicUserId>sip:+22{TEMPLATE}![1-4]!@windtre.it</publicUserId>
	<originalPublicUserId>sip:+22{TEMPLATE}![1-4]!@windtre.it</originalPublicUserId>
	<barringIndication>false</barringIndication>
	<defaultIndication>false</defaultIndication>
	<serviceProfileName>4</serviceProfileName>
	<irsId>IRSID1</irsId>
	<displayNamePrivacy>false</displayNamePrivacy>
	<isChildIMPU>false</isChildIMPU>
	<publicIdType>wildcardedIMPU</publicIdType>
</publicUserId>
    '''
    return template.replace('{TEMPLATE}', random_string)

def save_xml_to_file(filename, count=5):
    with open(filename, 'w') as file:
        for _ in range(count):
            random_string = generate_random_string()
            xml_snippet = generate_xml_template(random_string)
            file.write(xml_snippet)

# Generate and save the XML snippets to a file
save_xml_to_file('output.xml', count=750)  # Change the count to generate more instances

