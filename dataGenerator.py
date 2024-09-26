import random
import string

def generate_random_string(length=10):
    return ''.join(random.choice(string.ascii_letters) for _ in range(length))

def generate_xml_template(random_string):
    template = '''
<modification operation="add">
    <valueObject xsi:type="subscriber:PublicUserId">
        <originalPublicUserId>sip:ZTM{TEMPLATE}!.?.?.?!@nsn.com</originalPublicUserId>
        <defaultIndication>false</defaultIndication>
        <serviceProfileName>4</serviceProfileName>
        <irsId>IRS-01</irsId>
    </valueObject>
</modification>
    '''
    return template.replace('{TEMPLATE}', random_string)

def save_xml_to_file(filename, count=5):
    with open(filename, 'w') as file:
        for _ in range(count):
            random_string = generate_random_string()
            xml_snippet = generate_xml_template(random_string)
            file.write(xml_snippet)

# Generate and save the XML snippets to a file
save_xml_to_file('output.xml', count=145)  # Change the count to generate more instances

