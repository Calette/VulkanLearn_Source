from __future__ import print_function
import argparse
import json
import os
import os.path
import subprocess
import sys
import platform
import multiprocessing
import shlex
import shutil
import stat
import time

VERBOSE = False

def make_or_exist_dirs(path):
    "Wrapper for os.makedirs that tolerates the directory already existing"
    # Could use os.makedirs(path, exist_ok=True) if we drop python2
    if not os.path.isdir(path):
        os.makedirs(path)

def command_output(cmd, directory, fail_ok=False):
    """Runs a command in a directory and returns its standard output stream.
    Captures the standard error stream and prints it if error.
    Raises a RuntimeError if the command fails to launch or otherwise fails.
    """
    if VERBOSE:
        print('In {d}: {cmd}'.format(d=directory, cmd=cmd))
    p = subprocess.Popen(
        cmd, cwd=directory, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    (stdout, stderr) = p.communicate()
    if p.returncode != 0:
        print('*** Error ***\nstderr contents:\n{}'.format(stderr))
        if not fail_ok:
            raise RuntimeError('Failed to run {} in {}'.format(cmd, directory))
    if VERBOSE:
        print(stdout)
    return stdout

def Clone(name, repo_dir, url, retries=10, retry_seconds=60):
    print('Cloning {n} into {d}'.format(n=name, d=repo_dir))
    for retry in range(retries):
        make_or_exist_dirs(repo_dir)
        try:
            command_output(['git', 'clone', url, '.'], repo_dir)
            # If we get here, we didn't raise an error
            return
        except RuntimeError as e:
            print("Error cloning on iteration {}/{}: {}".format(retry + 1, retries, e))
            if retry + 1 < retries:
                if retry_seconds > 0:
                    print("Waiting {} seconds before trying again".format(retry_seconds))
                    time.sleep(retry_seconds)
                if os.path.isdir(repo_dir):
                    print("Removing old tree {}".format(repo_dir))
                    shutil.rmtree(repo_dir, onerror=on_rm_error)
                continue
    
            # If we get here, we've exhausted our retries.
            print("Failed to clone {} on all retries.".format(url))
            raise e

def Fetch(repo_dir, retries=10, retry_seconds=60):
    for retry in range(retries):
        try:
            command_output(['git', 'fetch', 'origin'], repo_dir)
            # if we get here, we didn't raise an error, and we're done
            return
        except RuntimeError as e:
            print("Error fetching on iteration {}/{}: {}".format(retry + 1, retries, e))
            if retry + 1 < retries:
                if retry_seconds > 0:
                    print("Waiting {} seconds before trying again".format(retry_seconds))
                    time.sleep(retry_seconds)
                continue
    
            # If we get here, we've exhausted our retries.
            print("Failed to fetch {} on all retries.".format(url))
            raise e
                
def Checkout(name, url, commit):
    repo_dir = os.getcwd() + '\\Include\\' + name
    print('Checking out {n} in {d}'.format(n=name, d=repo_dir))
    if not os.path.exists(os.path.join(repo_dir, '.git')):
        Clone(name, repo_dir, url)
    Fetch(repo_dir)
    #command_output(['git', 'checkout', commit], repo_dir)
    #print(command_output(['git', 'status'], repo_dir))

def main():
    return
    #Checkout('stb', 'https://github.com/nothings/stb.git', '')
    
if __name__ == '__main__':
    main()