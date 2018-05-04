#coding:utf-8

import os, sys

exec_loc = "sample"
time_limit = 30
argv = []

def run_test(dir_name):
    
    exists = os.path.exists
    mkdir = os.mkdir
    chdir = os.chdir
    def getsize(loc):
        # return "./$loc" file size number, unit: byte.
        if not exists(loc):
            return 0
        return int(os.popen("du -b {}".format(loc)).readlines()[0].split()[0])
    def copy(src, des):
        os.system("ln -rs {} {}".format(src, des))
    def remove(loc):
        os.system("rm -rf {}".format(loc))
    join = os.path.join

    ret = {}
    ret['time'] = ''
    ret['space'] = ''
    ret['state'] = 'runtime error'

    # clear tmp
    if (exists("tmp")):
        remove("tmp")
    mkdir("tmp")
    
    # prepare data
    copy(join(dir_name,"*"), "tmp")
    if (exists(join("tmp", "readme.txt"))):
        info = "".join(open(join("tmp", "readme.txt"), "r").readlines())
        print(info)
    
    # prepare program
    global exec_loc
    copy(exec_loc, "tmp")

    # set test environment
    pre_size = getsize("tmp")
    time_used = 0.0
    max_size = 0
    chdir("tmp")

    nw = 1
    while exists(str(nw)+".in"):
        
        # run your program!!!
        run_command = "timeout {} /usr/bin/time -o time.txt -f \"%e %s %w %K\" ./{} < ".format(str(time_limit), exec_loc) \
            +str(nw)+".in"+" 1>> user.out"
        ret_value = os.system(run_command)
        if (ret_value != 0):
            print("Time out")
            ret['state'] = 'time limit exceeded'
            chdir("..")
            return ret
        
        # get results
        nw_time_results = open("time.txt", "r").readlines()[0].split()
        nw_time_used = eval(nw_time_results[0])
        time_used += nw_time_used
        remove("time.txt")
        nw_size = getsize("../tmp") - getsize("user.out")
        max_size = max(max_size, nw_size)

        nw += 1

    if "-quite" not in argv:
        same = os.system("diff -b -B user.out ans.out") # 1>/dev/null")
    else:
        same = os.system("diff -b -B user.out ans.out 1>/dev/null")
    chdir("..")

    ret['time'] = time_used
    print("Time:\t"+str(time_used)+"s")
    ret['space'] = (max_size-pre_size)/1000
    print("Space:\t"+str((max_size-pre_size)/1000)+"K")
    if (same == 0):
        ret['state'] = 'accept'
        print("State:\t"+"ACCEPT!")
    else:
        ret['state'] = 'wrong answer'
        print("State:\t"+"Failed...")

    return ret

def main():
    global argv
    argv = sys.argv
    global exec_loc
    if (len(argv) > 1):
        exec_loc = argv[1]

    # test begin
    nw = 1
    result = []
    while (os.path.exists(str(nw))):
        bar = "--------------------"
        print(bar+" test {} ".format(str(nw))+bar)
        result.append(run_test(str(nw)))
        print()
        nw += 1

    if "-json" in argv:
        import json
        with open("result.json","w") as f:
            json.dump(result, f)

if __name__ == "__main__":
    main()
