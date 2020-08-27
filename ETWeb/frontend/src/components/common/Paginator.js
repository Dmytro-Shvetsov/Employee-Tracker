import React from 'react';
import { Pagination, PaginationItem, PaginationLink } from "reactstrap";

export default class Paginator extends React.Component {
    static DEFAULT_PAGE_STEP = 2;
    constructor(props) {
        super(props);
        this.state = {
            page: props.page,
            count: props.count,
            step: props.step || Paginator.DEFAULT_PAGE_STEP
        };
        this.onPageChange = props.onPageChange;
    }

    static getDerivedStateFromProps(props, state) {
        if (props.page !== state.page || props.count !== state.count) {
          return {
            page: props.page,
            step: props.step || Paginator.DEFAULT_PAGE_STEP,
            count: props.count,
          };
        }

        // Return null to indicate no change to state.
        return null;
    }

    generatePaginationRange() {
        let { page, count, step } = this.state;
        const range = [];
        for (let i = page - step; i < page + step && i <= count; i++) {
            if (i <= 0) {
                step += 1;
                continue;
            }
            range.push(i);
        }
        return range;
    }

    render() {
        const { page, count } = this.state;
        const range = this.generatePaginationRange();
        return (
            <Pagination className={this.props.className}>
                <PaginationItem disabled={page === 1}>
                    <PaginationLink first onClick={e => {e.preventDefault(); this.onPageChange(1); }}/>
                </PaginationItem>
                <PaginationItem disabled={page === 1}>
                    <PaginationLink previous onClick={e => this.onPageChange(page - 1)}/>
                </PaginationItem>
                {range.map(idx => (
                    <PaginationItem
                        active={idx === page}
                        key={idx}
                    >
                        <PaginationLink onClick={e => {e.preventDefault(); this.onPageChange(idx);}}>
                            {idx}
                        </PaginationLink>
                    </PaginationItem>
                ))}
                <PaginationItem disabled={page === count}>
                    <PaginationLink next onClick={e => {e.preventDefault(); this.onPageChange(page + 1);}}/>
                </PaginationItem>
                <PaginationItem disabled={page === count}>
                    <PaginationLink last onClick={e => {e.preventDefault(); this.onPageChange(count);}}/>
                </PaginationItem>
            </Pagination>
        );
    }
}