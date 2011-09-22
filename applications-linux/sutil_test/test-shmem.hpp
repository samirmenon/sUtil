/* This file is part of sUtil, a random collection of utilities.

sUtil is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

Alternatively, you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License, or (at your option) any later version.

sUtil is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License and a copy of the GNU General Public License along with
sUtil. If not, see <http://www.gnu.org/licenses/>.
 */
/*
 * test-shmem.hpp
 *
 *  Created on: Sep 09, 2011
 *
 *  Copyright (C) 2011, Samir Menon <smenon@stanford.edu>
 */

#ifndef TEST_SHMEM_HPP_
#define TEST_SHMEM_HPP_

#include <sutil/CSharedMemory.hpp>

//For getpid() and fork()
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

namespace sutil_test
{
  /** Structure that enables sharing 10 doubles */
  class SShmObj
  {
  public:
    char x[256];
  };

  /** Tests the dynamic shmem utility
   * @param arg_id : The id of the test */
  void test_shmem(const int arg_id)
  {
    bool flag = true;
    unsigned int test_id=0;
    pid_t pid_child = 1;
    try
    {
      //Create shared memory object, id 8080
      sutil::CSharedMemory<SShmObj, char> shmem(8080,'x');

      std::cout<<std::flush;

      pid_child = fork();

      if(0 == pid_child)
      {//Child process (client)
        sleep(1);//Sleep for 1 sec while the parent creates the shared memory

        std::cout<<"\nTest Result ("<<test_id++<<") Client: I am alive! My pid is: "<<pid_child;

        //3. Read the shared memory
        flag = shmem.shmRead();
        if(false == flag)
        { throw(std::runtime_error("Client: Failed to read shared memory"));  }
        else
        { std::cout<<"\nTest Result ("<<test_id++<<") Client: Read the shared memory"; }

        //4. Modify the shared memory
        std::cout<<"\nTest Result ("<<test_id++<<") Client: Setting the shared memory";
        strcpy(shmem.data_->x, "Namaste Parent-Server! I am your child-client.");
        std::cout<<"\nTest Result ("<<test_id++<<") Client: Data: "<< std::string(shmem.data_->x);

        //5. Signal the server and go to sleep
        std::cout<<"\nTest Result ("<<test_id++<<") Client: Signaling server";
        std::cout<<std::flush;
        *shmem.data_signal_ = 'a';

        while(*shmem.data_signal_ != 'b')
        { sleep(1); }

        //8. Read the server's message and terminate
        std::cout<<"\nTest Result ("<<test_id++<<") Client: Message from server: "<< std::string(shmem.data_->x);

        flag = shmem.shmDetach();
        if(false == flag)
        { throw(std::runtime_error("Client: Failed to read shared memory"));  }
        else
        { std::cout<<"\nTest Result ("<<test_id++<<") Client: Terminated. Adieu!"; }
      }
      else
      {//Parent process (server)
        std::cout<<"\nTest Result ("<<test_id++<<") Server: I spawned a child! Its pid is: "<<pid_child;

        //1. Create the shared memory
        flag = shmem.shmCreate();
        if(false == flag)
        { throw(std::runtime_error("Server: Failed to create shared memory"));  }
        else
        { std::cout<<"\nTest Result ("<<test_id++<<") Server: Created shared memory"; }

        //2. Wait for client to manipulate the shared memory and go to sleep
        std::cout<<"\nTest Result ("<<test_id++<<") Server: Waiting for client signal";

        std::cout<<std::flush;
        *shmem.data_signal_ = '0';
        while(*shmem.data_signal_ != 'a')
        { sleep(1); }

        //6. Read the client's message and manipulate the shared memory
        std::cout<<"\nTest Result ("<<test_id++<<") Server: Message from client: "<< std::string(shmem.data_->x);

        std::cout<<"\nTest Result ("<<test_id++<<") Server: Setting the shared memory";
        strcpy(shmem.data_->x, "Greetings child.");
        std::cout<<"\nTest Result ("<<test_id++<<") Server: Data: "<< std::string(shmem.data_->x);

        //7. Signal the client and get ready to terminate
        std::cout<<std::flush;
        *shmem.data_signal_ = 'b';

        //Checks whether the shmem signal is 'x', and terminates when it is
        while(!shmem.shmCheckDetach())
        { sleep(1); }
        std::cout<<"\nTest Result ("<<test_id++<<") Server: Client terminated, detaching shared memory";

        //9. Client terminated.
        std::cout<<"\nTest #"<<arg_id<<" (Shared Memory Test) Succeeded.";
      }
    }
    catch(std::exception& ee)
    {
      std::cout<<"\nTest Error ("<<test_id++<<") "<<ee.what();
      std::cout<<"\nTest #"<<arg_id<<" (Shared Memory Test) Failed";
    }

    if(0 == pid_child)//The child process doesn't need to continue.
    { exit(0);  }
  }

}
#endif /* TEST_SHMEM_HPP_ */
