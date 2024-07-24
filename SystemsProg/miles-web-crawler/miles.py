#!/usr/bin/env python3

''' miles.py - Web crawler to download files in parallel. '''

from typing import Iterator, Optional

import os
import concurrent.futures
import itertools
import re
import sys
import tempfile
import time
import urllib.parse

import requests

# Constants

FILE_REGEX = {
    'jpg': [r'<img.*src="?([^\" ]+.jpg)', r'<a.*href="?([^\" ]+.jpg)'],
    'mp3': [r'<audio.*src="?([^\" ]+.mp3)', r'<a.*href="?([^\" ]+.mp3)'],
    'pdf': [r'<a.*href="?([^\" ]+.pdf)'],
    'png': [r'<img.*src="?([^\" ]+.png)', r'<a.*href="?([^\" ]+.png)']
}

MEGABYTES   = 1<<20
DESTINATION = '.'
CPUS        = 1

# Functions

def usage(exit_status: int=0) -> None:
    ''' Print usgae message and exit. '''
    print(f'''Usage: miles.py [-d DESTINATION -n CPUS -f FILETYPES] URL

Crawl the given URL for the specified FILETYPES and download the files to the
DESTINATION folder using CPUS cores in parallel.

    -d DESTINATION      Save the files to this folder (default: {DESTINATION})
    -n CPUS             Number of CPU cores to use (default: {CPUS})
    -f FILETYPES        List of file types: jpg, mp3, pdf, png (default: all)

Multiple FILETYPES can be specified in the following manner:

    -f jpg,png
    -f jpg -f png''', file=sys.stderr)
    sys.exit(exit_status)

def resolve_url(base: str, url: str) -> str:
    ''' Resolve absolute url from base url and possibly relative url.

    >>> base = 'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/'
    >>> resolve_url(base, 'static/img/ostep.jpg')
    'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/img/ostep.jpg'

    >>> resolve_url(base, 'https://automatetheboringstuff.com/')
    'https://automatetheboringstuff.com/'
    '''
    if '//' in url:
        return url
    return urllib.parse.urljoin(base, url)


def extract_urls(url: str, file_types: list[str]) -> Iterator[str]:
    ''' Extract urls of specified file_types from url.

    >>> url = 'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/'
    >>> extract_urls(url, ['jpg']) # doctest: +ELLIPSIS
    <generator object extract_urls at ...>

    >>> len(list(extract_urls(url, ['jpg'])))
    2
    '''

    try:
        response = requests.get(url)
        response.raise_for_status()
    except requests.exceptions.RequestException:
        return
    
    html = response.text

    for ftype in file_types:
        for regex in FILE_REGEX[ftype]:
            for match in re.findall(regex, html):
                 yield resolve_url(url, match)

def download_url(url: str, destination: str=DESTINATION) -> Optional[str]:
    ''' Download url to destination folder.

    >>> url = 'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/img/ostep.jpg'
    >>> destination = tempfile.TemporaryDirectory()

    >>> path = download_url(url, destination.name)
    Downloading https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/static/img/ostep.jpg...

    >>> path # doctest: +ELLIPSIS
    '/tmp/.../ostep.jpg'

    >>> os.stat(path).st_size
    53696

    >>> destination.cleanup()
    ''' 
    print(f'Downloading {url}...')
    try:
        response = requests.get(url)
        response.raise_for_status()
    except requests.exceptions.RequestException:
        return None
    
    content = response.content
    name = os.path.basename(url)
    path = os.path.join(destination, name)
    
    with open(path, 'wb') as f:
        f.write(content)

    return path

def crawl(url: str, file_types: list[str], destination: str=DESTINATION, cpus: int=CPUS) -> None:
    ''' Crawl the url for the specified file type(s) and download all found
    files to destination folder.

    >>> url = 'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/'
    >>> destination = tempfile.TemporaryDirectory()
    >>> crawl(url, ['jpg'], destination.name) # doctest: +ELLIPSIS
    Files Downloaded: 2
    Bytes Downloaded: 0.07 MB
    Elapsed Time:     ... s
    Bandwidth:        0... MB/s

    >>> destination.cleanup()
    '''
    start_time = time.time()
    
    with concurrent.futures.ProcessPoolExecutor(cpus) as executor:
        urls = extract_urls(url, file_types)

        dsts = itertools.repeat(destination)

        files = executor.map(download_url, urls, dsts)
    
    total_time = time.time() - start_time
    num_files = 0
    total_mbytes: float = 0

    for path in files:
        if path:
            num_files += 1
            total_mbytes += os.stat(path).st_size / MEGABYTES

    bandwidth = total_mbytes / total_time

    print(f'Files Downloaded: {num_files}')
    print(f'Bytes Downloaded: {total_mbytes:.2f} MB')
    print(f'Elapsed Time:     {total_time:.2f} s')
    print(f'Bandwidth:        {bandwidth:.2f} MB/s')

# Main Execution

def main(arguments=sys.argv[1:]) -> None:
    ''' Process command line arguments, crawl URL for specified FILETYPES,
    download files to DESTINATION folder using CPUS cores.

    >>> url = 'https://www3.nd.edu/~pbui/teaching/cse.20289.sp24/'
    >>> destination = tempfile.TemporaryDirectory()
    >>> main(f'-d {destination.name} -f jpg {url}'.split()) # doctest: +ELLIPSIS
    Files Downloaded: 2
    Bytes Downloaded: 0.07 MB
    Elapsed Time:     0... s
    Bandwidth:        0... MB/s

    >>> destination.cleanup()
    '''
    destination = DESTINATION
    cpus = CPUS
    ftypes = []

    if len(arguments) == 0:
        usage(1)

    while len(arguments) > 0:
        arg = arguments.pop(0)
        if arg == "-d":
            destination = arguments.pop(0)
            if not (os.path.exists(destination)):
                os.makedirs(destination)
        elif arg == "-n":
            cpus = int(arguments.pop(0))
        elif arg == "-f":
            forms = arguments.pop(0)
            for form in forms.split(','):
                ftypes.append(form)
        elif arg == '-h':
            usage()
        elif '-' == arg[0] and len(arg) == 2:
            usage(1)
        else:
            url = arg
    
    if ftypes == []:
        ftypes = ['jpg','mp3', 'pdf', 'png']

    crawl(url, ftypes, destination, cpus)

if __name__ == '__main__':
    main()
