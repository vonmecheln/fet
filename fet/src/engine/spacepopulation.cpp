/*
File spacepopulation.cpp
*/

/*
Copyright 2002, 2003 Lalescu Liviu.

This file is part of FET.

FET is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

FET is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with FET; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "genetictimetable_defs.h"
#include "spacepopulation.h"
#include "rules.h"

#include <stdlib.h>

#include <q3valuestack.h>
//Added by qt3to4:
#include <QTextStream>

SpacePopulation::SpacePopulation(){
	this->initialized=false;
}

void SpacePopulation::init(Rules& r, int np, TimeChromosome& timeChromosome){
	assert(r.internalStructureComputed);

	assert(np<=MAX_POPULATION_SIZE && np>0);
	n=np;

	int i;
	for(i=0; i<n; i++)
		this->chromosomes[i].init(r);

	for(i=0; i<r.nInternalActivities; i++)
		if(timeChromosome.times[i]!=UNALLOCATED_TIME){
			this->days[i] = timeChromosome.times[i] % r.nDaysPerWeek;
			this->hours[i]= timeChromosome.times[i] / r.nDaysPerWeek;
		}
		else{
			this->days[i] = UNALLOCATED_TIME;
			this->hours[i]= UNALLOCATED_TIME;
		}

	this->sorted=false;
	this->bestFirst=false;
	this->initialized=true;	
}

int SpacePopulation::populationNumber(){
	return this->n;
}

void SpacePopulation::makeRoomsUnallocated(Rules& r){
	assert(this->initialized);

	assert(this->n > 0);

	for(int i=0; i < this->n; i++)
		this->chromosomes[i].makeRoomsUnallocated(r);

	this->sorted=0;
	this->bestFirst=0;
}

void SpacePopulation::makeRoomsRandom(Rules& r){
	assert(this->initialized);
	assert(this->n>0);

	for(int i=0; i<this->n; i++)
		this->chromosomes[i].makeRoomsRandom(r);

	this->sorted=false;
	this->bestFirst=false;
}

SpaceChromosome& SpacePopulation::bestChromosome(Rules& r){
	assert(this->initialized);

	if(this->bestFirst || this->sorted)
		return this->chromosomes[0];
		
	SpaceChromosome* c=&chromosomes[0];
	int j=-1;
	for(int i=1; i<this->n; i++)
		if(better(r, this->chromosomes[i], *c, this->days, this->hours)){
			c=&this->chromosomes[i];
			j=i;
		}
	if(j!=-1){
		SpaceChromosome c;
		c.copy(r, this->chromosomes[0]);
		this->chromosomes[0].copy(r, this->chromosomes[j]);
		this->chromosomes[j].copy(r, c);
	}
	
	this->bestFirst=true;
	return this->chromosomes[0];

/*	if(!this->sorted){
		this->sort(r, 0, n-1);
		this->sorted=true;
		this->bestFirst=true;
	}
	return this->chromosomes[0];*/
}

SpaceChromosome& SpacePopulation::worstChromosome(Rules& r){
	assert(this->initialized);

	if(!this->sorted){
		this->sort(r, 0, n-1);
		this->sorted=true;
		this->bestFirst=true;
	}
	return this->chromosomes[n-1];
}

SpaceChromosome& SpacePopulation::medianChromosome(Rules& r){
	assert(this->initialized);

	if(!this->sorted){
		this->sort(r, 0, n-1);
		this->sorted=true;
		this->bestFirst=true;
	}
	return this->chromosomes[n/2-1];
}

double SpacePopulation::totalHardFitness(Rules& r){
	assert(this->initialized);

	double hf=0;
	for(int i=0; i<this->n; i++)
		hf += this->chromosomes[i].hardFitness(r, this->days, this->hours);

	return hf;
}

double SpacePopulation::totalSoftFitness(Rules& r){
	assert(this->initialized);

	double sf=0;
	for(int i=0; i<this->n; i++)
		sf += this->chromosomes[i].softFitness(r, this->days, this->hours);

	return sf;
}

void SpacePopulation::evolve1(Rules& r){
//we double the number of individuals, by crossover, mutation1 or mutation2.
//Then we select the best half
	assert(this->initialized);

	for(int i=this->n; i<2*this->n; i++){
		//now choose: a crossover, mutation1 or mutation2?
		int z=rand()%100;
		if(z<METHOD1_MUTATION2_PROBABILITY){ //mutation2
			int p=rand()%this->n;
			this->chromosomes[i].copy(r, this->chromosomes[p]);
			this->chromosomes[i].mutate2(r);
		}
		else if(z<METHOD1_MUTATION1_PROBABILITY+METHOD1_MUTATION2_PROBABILITY){ //mutation1
			int p=rand()%this->n;
			this->chromosomes[i].copy(r, this->chromosomes[p]);
			this->chromosomes[i].mutate1(r);
		}
		else{ //crossover
			int p,q;
			do{
				p=rand()%n;
				q=rand()%n;
			}while(p==q); 
			this->chromosomes[i].crossover(r, this->chromosomes[p], this->chromosomes[q]);
		}
	}

	this->n*=2;
	this->sort(r, 0, this->n-1);
	this->sorted = true;
	this->bestFirst = true;	
	this->n/=2;
}

void SpacePopulation::sort(Rules& ru, int left, int right){
	assert(this->initialized);

	/*
	if(l>=r)
		return;
	int i=l-1,
		j=r+1,
		p=l+rand()%(r-l+1),
		hpivot=chromosomes[p].HFitness(),
		spivot=chromosomes[p].SFitness(); 
	for(;;){
		do i++; while (Better(chromosomes[i].HFitness(), chromosomes[i].SFitness(), hpivot, spivot));
		do j--; while (Better(hpivot, spivot, chromosomes[j].HFitness(), chromosomes[j].SFitness()));
		
		if(i<j){
			Chromosome k;
			
			k=chromosomes[i]; 
			chromosomes[i]=chromosomes[j]; 
			chromosomes[j]=k;
		}
		else
			break;
	}
	Sort(l, j);
	Sort(j+1, r);*/

	Q3ValueStack<int> stack1;
	Q3ValueStack<int> stack2;
	stack1.push(left);
	stack2.push(right);

	for(;;){
		int l, r;
		if(stack1.isEmpty()){
			assert(stack2.isEmpty());
			break;
		}
		assert(!stack2.isEmpty());
		l=stack1.pop();
		r=stack2.pop();
		assert(l < r);

		int i=l-1, j=r+1, p=l+rand()%(r-l+1),
		 hpivot=this->chromosomes[p].hardFitness(ru, this->days, this->hours),
		 spivot=this->chromosomes[p].softFitness(ru, this->days, this->hours);

		for(;;){
			do i++; while (better(this->chromosomes[i].hardFitness(ru, this->days, this->hours), this->chromosomes[i].softFitness(ru, this->days, this->hours), hpivot, spivot));
			do j--; while (better(hpivot, spivot, this->chromosomes[j].hardFitness(ru, this->days, this->hours), this->chromosomes[j].softFitness(ru, this->days, this->hours)));
			
			if(i<j){
				SpaceChromosome k;
				
				/*k=chromosomes[i]; 
				chromosomes[i]=chromosomes[j]; 
				chromosomes[j]=k;*/
				k.copy(ru, this->chromosomes[i]); 
				this->chromosomes[i].copy(ru, this->chromosomes[j]);
				this->chromosomes[j].copy(ru, k);
			}
			else
				break;
		}

		if(l < j){
			stack1.push(l);
			stack2.push(j);
		}
		if(j+1 < r){
			stack1.push(j+1);
			stack2.push(r);
		}
	}
}

void SpacePopulation::evolve2(Rules& r){
	assert(this->initialized);

	//The population is changed without elitism (although I keep the best chromosome from the population)
	//Selection is based on 3 tournament.

	int i,k;
	for(i=this->n; i<2*this->n-1; i++){
		int c1, c2, c3, t;
		do{
			c1=rand()%this->n;
			c2=rand()%this->n;
		}while(c1==c2);
		do{
			c3=rand()%this->n;
		}while(c1==c3 || c2==c3);

		if(better(r, this->chromosomes[c2], this->chromosomes[c1], this->days, this->hours))
			t=c1, c1=c2, c2=c1; //Exchange(c1, c2);
		if(better(r, this->chromosomes[c3], this->chromosomes[c1], this->days, this->hours))
			t=c1, c1=c3, c3=c1; //Exchange(c1, c3);
		if(better(r, this->chromosomes[c3], this->chromosomes[c2], this->days, this->hours))
			t=c3, c3=c2, c2=c3; //Exchange(c2, c3);
		
		k=rand()%100;
		if(k<METHOD2_MUTATION1_PROBABILITY){
			//Mutation1
			//chromosomes[i]=chromosomes[c1];
			this->chromosomes[i].copy(r, this->chromosomes[c1]);
			this->chromosomes[i].mutate1(r);
		}
		else if(k<METHOD2_MUTATION1_PROBABILITY+METHOD2_MUTATION2_PROBABILITY){
			//Mutation2
			//chromosomes[i]=chromosomes[c1];
			this->chromosomes[i].copy(r, this->chromosomes[c1]);
			this->chromosomes[i].mutate2(r);
		}
		else if(k<METHOD2_CROSSOVER_PROBABILITY+METHOD2_MUTATION1_PROBABILITY+METHOD2_MUTATION2_PROBABILITY){
			this->chromosomes[i].crossover(r, this->chromosomes[c1], this->chromosomes[c2]);
		}
		else{
			//chromosomes[i]=chromosomes[c1];
			this->chromosomes[i].copy(r, this->chromosomes[c1]);
		}
	}

	if(!this->sorted && !this->bestFirst){	//We'll find the best chromosome and put it first
		k=0;
		for(i=1; i<this->n; i++)
			if(better(r, this->chromosomes[i], this->chromosomes[k], this->days, this->hours))
				k=i;

		if(k!=0){
			SpaceChromosome tmp;
			/*tmp=chromosomes[0]; 
			chromosomes[0]=chromosomes[k]; 
			chromosomes[k]=tmp;*/
			tmp.copy(r, this->chromosomes[0]);
			this->chromosomes[0].copy(r, this->chromosomes[k]);
			this->chromosomes[k].copy(r, tmp);
		}
	}

	//Now we kill the old population (without the best chromosome) and put instead the new population
	for(i=1; i < this->n; i++)
		//chromosomes[i]=chromosomes[i+n-1];
		this->chromosomes[i].copy(r, this->chromosomes[i+n-1]);
	
	this->sorted = false; //because we altered the order

	//Now we find the best chromosome and put it first
	k=0;
	for(i=1; i<this->n; i++)
		if(better(r, this->chromosomes[i], this->chromosomes[k], this->days, this->hours))
			k=i;
	
	//Exchange chromosome 0 with chrom k
	if(k!=0){ //this test is really important
		SpaceChromosome tmp;
		/*tmp=chromosomes[0]; 
		chromosomes[0]=chromosomes[k]; 
		chromosomes[k]=tmp;*/
		tmp.copy(r, this->chromosomes[0]);
		this->chromosomes[0].copy(r, this->chromosomes[k]);
		this->chromosomes[k].copy(r, tmp);
	}

	this->bestFirst = true;
}

void SpacePopulation::read(Rules& r, QTextStream &tis)
{
	tis>>this->n;
	for(int i=0; i<this->n; i++)
		this->chromosomes[i].read(r, tis);
}

bool SpacePopulation::read(Rules& r, const QString& filename)
{
	QFile file(filename);
	
	if(!file.open(QIODevice::ReadOnly))
		return false;
		
	QTextStream tis(&file);
	this->read(r, tis);
	file.close();
	return true;
}

void SpacePopulation::write(Rules& r, QTextStream &tos)
{
	tos<<this->n<<endl<<endl;
	for(int i=0; i<this->n; i++){
		this->chromosomes[i].write(r, tos);
		tos<<endl;
	}
}

void SpacePopulation::write(Rules& r, const QString& filename)
{
	QFile file(filename);
	if(!file.open(QIODevice::WriteOnly))
		assert(0);
	QTextStream tos(&file);
	this->write(r, tos);
	file.close();
}
